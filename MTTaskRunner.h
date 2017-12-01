#ifndef MTTASKRUNNER_H_INCLUDED
#define MTTASKRUNNER_H_INCLUDED

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <deque>

#include "MTTask.h"

using namespace std;

typedef enum {
    RUNNER_NO_ERROR    = 0,
    RUNNER_INVALID     = 1,
    RUNNER_LOCK_ERR    = 2,
    RUNNER_FULL        = 3,
    RUNNER_SHUTDOWN    = 4,
    RUNNER_THREAD_ERR  = 5
} RUNNER_STATE;

typedef enum {
    DELAY_CMD_RUUNING        = 0,
    DELAY_CMD_EXIT_NO_WAIT   = 1,
    DELAY_CMD_EXIT_GRACEFULL = 2
} DELAY_CMD_TYPE;

class MTTaskRunner {
public:
    uint8_t         mMaxWorkerSize;
    uint8_t         mMaxTaskSize;
    pthread_mutex_t mMutex;
    pthread_cond_t  mNotify;
    DELAY_CMD_TYPE  mDelayCMD;
    RUNNER_STATE    mRunnerType;
    std::deque<MTTask*>      mTaskQueue;
    std::vector<MTTask*>     mTaskQueueRun;
    std::vector<pthread_t*>  mThreadQueue;
public:
    MTTaskRunner(int runner_size, int task_size);
    ~MTTaskRunner();
    int start();
    int addTask(TASK_PRIORITY prio, TASK_FUNC task_func, void *args, uint8_t flags);
    int addTask(TASK_PRIORITY prio, MTTask* task);
    int deleteTask(MTTask* task);
    int flush(uint8_t flags);
};

#endif // MTTASKRUNNER_H_INCLUDED
