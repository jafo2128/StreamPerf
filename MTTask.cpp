#include "MTTask.h"

MTTask::MTTask() {
    mArgs     = NULL;
    mFlags    = 0;
    mCancel   = false;
    mTid      = 0;
    mPriority = PRIORITY_FIFO;
    mTaskFunc = NULL;
}

MTTask::~MTTask() {
    mArgs     = NULL;
    mFlags    = 0;
    mCancel   = false;
    mTid      = 0;
    mPriority = PRIORITY_FIFO;
    mTaskFunc = NULL;
}

void MTTask::cancel() {
    mCancel   = true;
}

int  MTTask::doTask(void* args) {
    return -1;
}
