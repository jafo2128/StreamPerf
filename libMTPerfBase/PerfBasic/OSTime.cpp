#include "OSTime.h"

#ifdef WIN32
#include <windows.h> //Sleep(millisecond)
#else
#include <unistd.h> //usleep(microsecond)
#endif /*WIN32*/

/*
 * usleep function is in <unistd.h>
 * usleep can hang up the process for a while
 * usleep doesn't work in windows operation system
 */
void mt_msleep(uint32_t ms){
#ifdef WIN32
    Sleep(ms); //millisecond
#else
    usleep(ms*1000)
#endif // WIN32
}

/*
 * usleep function is in <unistd.h>
 * gettimeofday doesn't work WELL in windows operation system
 */

int mt_gettimeofday(struct timeval *time_now, void *tz/*time_zone*/)
{
#ifdef WIN32
    time_t clock;
    struct tm tm;
    SYSTEMTIME clk_now;
    GetLocalTime(&clk_now);
    tm.tm_year   = clk_now.wYear - 1900;
    tm.tm_mon    = clk_now.wMonth - 1;
    tm.tm_mday   = clk_now.wDay;
    tm.tm_hour   = clk_now.wHour;
    tm.tm_min    = clk_now.wMinute;
    tm.tm_sec    = clk_now.wSecond;
    tm.tm_isdst  = -1;
    clock = mktime(&tm);
    time_now->tv_sec  = clock;
    time_now->tv_usec = clk_now.wMilliseconds * 1000;
    return (0);
#else
    return gettimeofday(time_now, tz);
#endif
}

