#ifndef MT_OS_TIME_H_INCLUDE
#define MT_OS_TIME_H_INCLUDE

#include <stdint.h>
#include <time.h>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#define US_TO_NS 1000
#define SEC_TO_US 1000000LL
#define SEC_TO_NS 1000000000LL	/* too big for enum/const on some platforms */

/*
 * usleep function is in <unistd.h>
 * usleep can hang up the process for a while
 * usleep doesn't work in windows operation system
 */
extern void mt_msleep(uint32_t ms);

extern int mt_gettimeofday(struct timeval *time_now, void *tz/*time_zone*/);

#endif // MT_OS_TIME_H_INCLUDE

