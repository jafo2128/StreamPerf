#include "MTPerfTaskQos.h"
#include "MTLog.h"
#include "MTTimer.h"

#define TAG "MTPerfTaskQos"

static int _timer_flag = 0;
static void timer_proc( TimerClientData timer_data, struct timeval* now ){
   _timer_flag = 1;
   MTLog::LogEx(TAG, __FUNCTION__, "timer_proc Callback _timer_flag=%d", _timer_flag);
}

int MTPerfTaskQos::doTask(void* args) {
    Timer *timer = NULL;
    struct timeval base, now;

    MTLog::LogEx(TAG, __FUNCTION__, "Begin.......MTPerfTaskQos.....");

    gettimeofday(&now, NULL );
    gettimeofday(&base, NULL );
    timer = timer_create((struct timeval*)&base, timer_proc, JunkClientData, 3*SEC_TO_US, 0);
    MTLog::LogDebug(TAG, __FUNCTION__, 1/*debug*/, "<%p>timeval_diff(us) = %d", this, timeval_diff(&base, &now));
    if(NULL == timer) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to create timer");
        goto TAG_ERROR;
    }
    msleep(2000);

    gettimeofday(&now, NULL );
    timer_run((struct timeval*)&now);
    MTLog::LogDebug(TAG, __FUNCTION__, 1/*debug*/, "<%p>timeval_diff(us) = %u", this, timeval_diff(&base, &now));
    if (_timer_flag) {
        MTLog::LogEx(TAG, __FUNCTION__, "timer should not have expired");
        goto TAG_ERROR;
    }
    msleep(1000);

    gettimeofday(&now, NULL );
    timer_run((struct timeval*)&now);
    MTLog::LogDebug(TAG, __FUNCTION__, 1/*debug*/, "<%p>timeval_diff(us) = %u", this, timeval_diff(&base, &now));
    if (!_timer_flag) {
        MTLog::LogEx(TAG, __FUNCTION__, "timer should have expired");
        goto TAG_ERROR;
    }
    timer_destroy();

    MTLog::LogEx(TAG, __FUNCTION__, "Done.......MTPerfTaskQos.....");

TAG_ERROR:
    return 0;
}
