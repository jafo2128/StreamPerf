#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

#include "Define.h"

#define MAX_THREADS 64
#define MAX_QUEUE   65536

#define TAG "MTTaskRunner"

MTTaskRunner::MTTaskRunner(int runner_size, int task_size) {
    mMaxWorkerSize = runner_size;
    mMaxTaskSize   = task_size;
    if(runner_size <= 0 || runner_size > MAX_THREADS) {
        MTLog::LogEx(TAG, __FUNCTION__, "invalid runner_size(%d)", runner_size);
        mMaxWorkerSize = MAX_THREADS;
    }
    if(task_size <= 0 || task_size > MAX_QUEUE) {
        MTLog::LogEx(TAG, __FUNCTION__, "invalid task_size(%d)", task_size);
        mMaxTaskSize = MAX_THREADS;
    }
    MTLog::LogEx(TAG, __FUNCTION__, "mMaxWorkerSize(%d) mMaxTaskSize(%d) ",
                                     mMaxWorkerSize, mMaxTaskSize);

    if(0 != pthread_mutex_init(&mMutex, NULL)) {
       MTLog::LogEx(TAG, __FUNCTION__, "Fail to pthread_mutex_init");
    }
    if(0 != pthread_cond_init(&mNotify, NULL)) {
       MTLog::LogEx(TAG, __FUNCTION__, "Fail to pthread_cond_init");
    }

    mTaskQueue.clear();
    mTaskQueueRun.clear();
    mThreadQueue.clear();
}

MTTaskRunner::~MTTaskRunner(){
    this->flush(DELAY_CMD_EXIT_NO_WAIT);
}

static long int thread_tid() {
    #if 0
    #define __NR_gettid 186
    return (long int)syscall(__NR_gettid);
    #else
    return (long int)(pthread_self()+0xF1);
    #endif
}

static void *Worker_Loop(void* taskRunner)
{
    MTTask*       task = NULL;
    MTTaskRunner* runner = (MTTaskRunner *)taskRunner;

    runner->mDelayCMD = DELAY_CMD_RUUNING;
    MTLog::LogEx(TAG, __FUNCTION__, "Thread Pool's Worker_Loop [pid=%ld]", thread_tid());
    do {
        /* Lock must be taken to wait on conditional variable */
        pthread_mutex_lock(&(runner->mMutex));

        /* Wait on condition variable, check for spurious wakeups.
           When returning from pthread_cond_wait(), we own the lock. */
        while((0 == runner->mThreadQueue.size()) && (DELAY_CMD_RUUNING == runner->mDelayCMD)) {
            pthread_cond_wait(&(runner->mNotify), &(runner->mMutex));
        }

        if((DELAY_CMD_EXIT_GRACEFULL == runner->mDelayCMD) && (0 == runner->mThreadQueue.size())) {
            break;
        } else if(DELAY_CMD_EXIT_NO_WAIT == runner->mDelayCMD) {
            break;
        }

        if(runner->mTaskQueue.size()>0) {
            task = (MTTask*)(runner->mTaskQueue.front());
        }
        pthread_mutex_unlock(&(runner->mMutex));

        /* Doing Task */
        if(NULL != task) {
            /* removes the first element in the deque container */
            runner->mTaskQueue.pop_front();

            if(NULL != task->mTaskFunc) {
                (*(task->mTaskFunc))(task->mArgs, task->mFlags);
            } else {
                task->doTask(task->mArgs);
            }
            MTLog::LogEx(TAG, __FUNCTION__, "Task(%x) is done, ok", task);
            delete task;
            task = NULL;
        }
    } while(true);
    MTLog::LogEx(TAG, __FUNCTION__, "Thread Pool's Worker_Loop Exit....[pid=%ld]", thread_tid());
    pthread_mutex_unlock(&(runner->mMutex));
    pthread_exit(NULL);
    return(NULL);
}

/* Start worker threads */
int MTTaskRunner::start() {
    for(uint8_t i = 0; i < mMaxWorkerSize; i++) {
        pthread_t* ptrd = (pthread_t*)malloc(sizeof(pthread_t));
        if(0 != pthread_create(ptrd, NULL, Worker_Loop, (void*)this)) {
            MTLog::LogEx(TAG, __FUNCTION__, "Fail to pthread_create(%x), flush....", ptrd);
            flush(DELAY_CMD_EXIT_NO_WAIT);
            return -1;
        } else {
            MTLog::LogEx(TAG, __FUNCTION__, "pthread_create ok, add to mThreadQueue(%x)", ptrd);
            mThreadQueue.push_back(ptrd);
        }
    }
    return 0;
}

int MTTaskRunner::addTask(TASK_PRIORITY prio, TASK_FUNC task_func, void *args, uint8_t flags) {
    MTTask* task = new MTTask();
    task->mPriority  = prio;
    task->mTaskFunc  = task_func;
    task->mArgs      = args;
    task->mFlags     = flags;
    return addTask(prio, task);
}

int MTTaskRunner::addTask(TASK_PRIORITY prio, MTTask* task) {
    int err = RUNNER_NO_ERROR;
    if(pthread_mutex_lock(&mMutex) != 0) {
        return RUNNER_LOCK_ERR;
    }
    do {
        /* Are we full ? */
        if(mTaskQueue.size() > mMaxTaskSize) {
            err = RUNNER_FULL;
            break;
        }
       /* Are we shutting down ? */
        if(mDelayCMD != DELAY_CMD_RUUNING ) {
            err = RUNNER_SHUTDOWN;
            break;
        }
        switch(prio) {
        case PRIORITY_FIFO:
            mTaskQueue.push_back(task);
            break;
        case PRIORITY_HIGH:
            mTaskQueue.push_front(task);
            break;
        default:
            mTaskQueue.push_back(task);
            break;
        }
        /* pthread_cond_broadcast */
        if(0 != pthread_cond_signal(&mNotify)) {
            err = RUNNER_LOCK_ERR;
            break;
        }
    } while(0);

    if(0 != pthread_mutex_unlock(&mMutex)) {
        err = RUNNER_LOCK_ERR;
    }
    return err;
}

int MTTaskRunner::deleteTask(MTTask* task) {
    int err = RUNNER_NO_ERROR;
    if(pthread_mutex_lock(&mMutex) != 0) {
        return RUNNER_LOCK_ERR;
    }
    do {
        if(NULL != task) {
            task->mCancel = true;
        }
    } while(0);

    if(0 != pthread_mutex_unlock(&mMutex)) {
        err = RUNNER_LOCK_ERR;
    }
    return err;
}

int MTTaskRunner::flush(uint8_t flags) {
    int err = 0;

    MTLog::LogEx(TAG, __FUNCTION__, "begin");
    if(0 != pthread_mutex_lock(&mMutex)) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to pthread_mutex_lock");
        return RUNNER_LOCK_ERR;
    }

    if(0 == mThreadQueue.size() ) {
        MTLog::LogEx(TAG, __FUNCTION__, "Have none worker threads");
        return RUNNER_SHUTDOWN;
    }

    mDelayCMD = (flags & DELAY_CMD_EXIT_GRACEFULL) ?
            DELAY_CMD_EXIT_GRACEFULL : DELAY_CMD_EXIT_NO_WAIT;

    if(0 != pthread_cond_broadcast(&mNotify)) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to pthread_cond_broadcast");
        err = RUNNER_LOCK_ERR;
    }
    if(0 != pthread_mutex_unlock(&mMutex)) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to pthread_mutex_unlock");
        err = RUNNER_LOCK_ERR;
    }

    /* Join all worker thread */
    for(uint8_t i = 0; i < mThreadQueue.size(); i++) {
        if(0 != pthread_join(*mThreadQueue[i], NULL)) {
            MTLog::LogEx(TAG, __FUNCTION__, "fail to pthread_join(%x)", mThreadQueue[i]);
            err = RUNNER_THREAD_ERR;
        }else{
            MTLog::LogEx(TAG, __FUNCTION__, "pthread_join(%x) ok...", mThreadQueue[i]);
        }
    }
    mThreadQueue.clear();

    return err;
}
