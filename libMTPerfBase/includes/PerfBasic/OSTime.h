#ifndef MT_OS_TIME_H_INCLUDE
#define MT_OS_TIME_H_INCLUDE

#include <stdint.h>
#include <time.h>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

extern void mt_msleep(uint32_t ms);

extern int mt_gettimeofday(struct timeval *time_now, void *tz/*time_zone*/);

#endif // MT_OS_TIME_H_INCLUDE

