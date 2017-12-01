#ifndef MTTASK_H_INCLUDED
#define MTTASK_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

typedef void (*TASK_FUNC)(void *, uint32_t);

typedef enum {
    PRIORITY_FIFO     = 0,
    PRIORITY_NORMAL   = 1,
    PRIORITY_HIGH     = 2,
} TASK_PRIORITY;

class MTTask {
public:
    uint32_t      mTid;
    TASK_PRIORITY mPriority;
    TASK_FUNC     mTaskFunc;
    void          *mArgs;
    uint32_t      mFlags;
    bool          mCancel;
public:
    MTTask();
    virtual ~MTTask();
    virtual int  doTask(void* args);
    virtual void cancel();
};

#endif // MTTASK_H_INCLUDED


