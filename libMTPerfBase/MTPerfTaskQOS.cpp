#include "MTPerfTaskQOS.h"
#include "MTLog.h"
#include "MTTimer.h"
#include "OSTime.h"

#define TAG "MTPerfTaskQOS"

static int _timer_flag = 0;
static void timer_proc( TimerClientData timer_data, struct timeval* now ){
   _timer_flag = 1;
   MTLog::LogEx(TAG, __FUNCTION__, "timer_proc Callback _timer_flag=%d", _timer_flag);
}

int MTPerfTaskQOS::doTask(void* args) {
    Timer *timer = NULL;
    struct timeval base, now;

    MTLog::LogEx(TAG, __FUNCTION__, "task doing, boss...");

    for(int i = 0; i < 50; i++) {
        gettimeofday(&base, NULL );
        mt_msleep(100);
        gettimeofday(&now, NULL );
        MTLog::LogDebug(TAG, __FUNCTION__, 1/*debug*/, "<%p>timeval_diff(idx=%d) = %lld", this, i, timeval_diff(&base, &now)/1000);
    }

    gettimeofday(&now, NULL );
    gettimeofday(&base, NULL );
    timer = timer_create((struct timeval*)&base, timer_proc, JunkClientData, 3*SEC_TO_US, 0);
    MTLog::LogDebug(TAG, __FUNCTION__, 1/*debug*/, "<%p>timeval_diff(us) = %d", this, timeval_diff(&base, &now));
    if(NULL == timer) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to create timer");
        goto TAG_ERROR;
    }
    mt_msleep(2000);

    gettimeofday(&now, NULL );
    timer_run((struct timeval*)&now);
    MTLog::LogDebug(TAG, __FUNCTION__, 1/*debug*/, "<%p>timeval_diff(us) = %u", this, timeval_diff(&base, &now));
    if (_timer_flag) {
        MTLog::LogEx(TAG, __FUNCTION__, "timer should not have expired");
        goto TAG_ERROR;
    }
    mt_msleep(1000);

    gettimeofday(&now, NULL );
    timer_run((struct timeval*)&now);
    MTLog::LogDebug(TAG, __FUNCTION__, 1/*debug*/, "<%p>timeval_diff(us) = %u", this, timeval_diff(&base, &now));
    if (!_timer_flag) {
        MTLog::LogEx(TAG, __FUNCTION__, "timer should have expired");
        goto TAG_ERROR;
    }
    timer_destroy();

    MTLog::LogEx(TAG, __FUNCTION__, "task done, boss...");

TAG_ERROR:
    return 0;
}
