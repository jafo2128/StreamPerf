/*
 *      Copyright (C) 2017-2020 MediaTime
 *      http://media-tm.com (shareviews@sina.com)
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with MediaTime; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 *   original author: Based on timers.h by Jef Poskanzer. Used with permission.
 *   Second Author: iperf owned by the U.S. Department of Energy
 *       Modified by: shareviews@sina.com (2017-12-XX)
 */

#ifndef MTTIMER_H_INCLUDED
#define MTTIMER_H_INCLUDED

#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>

/* TimerClientData is an opaque value that tags along with a timer.  The
** client can use it for whatever, and it gets passed to the callback when
** the timer triggers.
*/
typedef union
{
    void* p;
    int i;
    long l;
} TimerClientData;

extern TimerClientData JunkClientData;	/* for use when you don't care */

/* The TimerProc gets called when the timer expires.  It gets passed
** the TimerClientData associated with the timer, and a timeval in case
** it wants to schedule another timer.
*/
typedef void TimerProc( TimerClientData client_data, struct timeval* now);

/* The Timer struct. */
typedef struct TimerStruct
{
    TimerProc* timer_proc;
    TimerClientData client_data;
    int64_t usecs;
    int periodic;
    struct timeval time;
    struct TimerStruct* prev;
    struct TimerStruct* next;
    int hash;
} Timer;

extern uint64_t timeval_diff(struct timeval *tv_a, struct timeval *tv_b);

/* Set up a timer, either periodic or one-shot. Returns (Timer*) 0 on errors. */
extern Timer* timer_create( struct timeval* now, TimerProc* timer_proc,
                            TimerClientData client_data, int64_t usecs, int periodic );

/* Returns a timeout indicating how long until the next timer triggers.  You
** can just put the call to this routine right in your select().  Returns
** (struct timeval*) 0 if no timers are pending.
*/
extern struct timeval* timer_timeout( struct timeval* now ) /* __attribute__((hot)) */;

/* Run the list of timers. Your main program needs to call this every so often,
** or as indicated by timer_timeout().
*/
extern void timer_run( struct timeval* now ) /* __attribute__((hot)) */;

/* Reset the clock on a timer, to current time plus the original timeout. */
extern void timer_reset( struct timeval* now, Timer* timer );

/* Deschedule a timer.  Note that non-periodic timers are automatically
** descheduled when they run, so you don't have to call this on them.
*/
extern void timer_cancel( Timer* timer );

/* Clean up the timers package, freeing any unused storage. */
extern void timer_cleanup( void );

/* Cancel all timers and free storage, usually in preparation for exiting. */
extern void timer_destroy( void );




#endif // MTTIMER_H_INCLUDED
