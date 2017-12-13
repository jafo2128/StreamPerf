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
 *   original author: Based on timers.c by Jef Poskanzer. Used with permission.
 *   Second Author: iperf owned by the U.S. Department of Energy
 *       Modified by: shareviews@sina.com (2017-12-XX)
 */

#include <sys/types.h>
#include <stdlib.h>

#include "MTTimer.h"

#ifdef WIN32
#include <windows.h> //Sleep(millisecond)
#else
#include <unistd.h> //usleep(microsecond)
#endif /*WIN32*/

static Timer* timers = NULL;
static Timer* free_timers = NULL;

TimerClientData JunkClientData;

/* This is an efficiency tweak.  All the routines that need to know the
** current time get passed a pointer to a struct timeval.  If it's non-NULL
** it gets used, otherwise we do our own gettimeofday() to fill it in.
** This lets the caller avoid extraneous gettimeofday()s when efficiency
** is needed, and not bother with the extra code when efficiency doesn't
** matter too much.
*/
static void getnow( struct timeval* time_old, struct timeval* time_now ) {
    if ( time_old != NULL ) {
        *time_now = *time_old;
    } else {
        (void) gettimeofday(time_now, NULL );
    }
}

void msleep(uint32_t ms){
#ifdef WIN32
    Sleep(ms); //millisecond
#else
    usleep(ms*1000)
#endif // WIN32
}

uint64_t timeval_diff(struct timeval * tv_a, struct timeval * tv_b)
{
    uint64_t time1, time2;

    time1 = tv_a->tv_sec*1000000 + tv_a->tv_usec;
    time2 = tv_b->tv_sec*1000000 + tv_b->tv_usec;

    time1 = time1 - time2;
    if (time1 < 0)
        time1 = -time1;
    return time1;
}

static void list_add( Timer* t )
{
    Timer* t2;
    Timer* t2prev;

    if ( timers == NULL ) {
        /* The list is empty. */
        timers = t;
        t->prev = t->next = NULL;
    } else {
        if ( t->time.tv_sec < timers->time.tv_sec ||
            ( t->time.tv_sec == timers->time.tv_sec &&
            t->time.tv_usec < timers->time.tv_usec ) ) {
                /* The new timer goes at the head of the list. */
                t->prev = NULL;
                t->next = timers;
                timers->prev = t;
                timers = t;
        } else {
            /* Walk the list to find the insertion point. */
            for ( t2prev = timers, t2 = timers->next; t2 != NULL; t2prev = t2, t2 = t2->next ) {
                if ( t->time.tv_sec < t2->time.tv_sec ||
                     ( t->time.tv_sec == t2->time.tv_sec && t->time.tv_usec < t2->time.tv_usec ) ) {
                    /* Found it. */
                    t2prev->next = t;
                    t->prev = t2prev;
                    t->next = t2;
                    t2->prev = t;
                    return;
		         }
	         }
            /* Oops, got to the end of the list.  Add to tail. */
            t2prev->next = t;
            t->prev = t2prev;
            t->next = NULL;
	    }
    }
}


static void list_remove( Timer* t )
{
    if ( t->prev == NULL )
        timers = t->next;
    else
        t->prev->next = t->next;
        if ( t->next != NULL )
        t->next->prev = t->prev;
}


static void list_resort( Timer* t )
{
    /* Remove the timer from the list. */
    list_remove( t );
    /* And add it back in, sorted correctly. */
    list_add( t );
}


static void add_usecs( struct timeval* t, int64_t usecs )
{
    t->tv_sec += usecs / 1000000L;
    t->tv_usec += usecs % 1000000L;
    if ( t->tv_usec >= 1000000L ) {
        t->tv_sec += t->tv_usec / 1000000L;
        t->tv_usec %= 1000000L;
    }
}

Timer* timer_create( struct timeval* time_now, TimerProc* timer_proc,
                     TimerClientData client_data, int64_t usecs, int periodic )
{
    struct timeval now;
    Timer* t;

    getnow( time_now, &now );

    if ( free_timers != NULL ) {
        t = free_timers;
        free_timers = t->next;
    } else {
        t = (Timer*) malloc( sizeof(Timer) );
        if ( t == NULL )
            return NULL;
    }

    t->timer_proc = timer_proc;
    t->client_data = client_data;
    t->usecs = usecs;
    t->periodic = periodic;
    t->time = now;
    add_usecs( &t->time, usecs );
    /* Add the new timer to the active list. */
    list_add( t );

    return t;
}

struct timeval* timer_timeout( struct timeval* time_now )
{
    struct timeval now;
    int64_t usecs;
    static struct timeval timeout;

    getnow( time_now, &now );
    /* Since the list is sorted, we only need to look at the first timer. */
    if ( timers == NULL )
	return NULL;
    usecs = ( timers->time.tv_sec - now.tv_sec ) * 1000000LL +
	    ( timers->time.tv_usec - now.tv_usec );
    if ( usecs <= 0 )
	usecs = 0;
    timeout.tv_sec = usecs / 1000000LL;
    timeout.tv_usec = usecs % 1000000LL;
    return &timeout;
}

void timer_run( struct timeval* time_now )
{
    struct timeval now;
    Timer* t;
    Timer* next;

    getnow( time_now, &now );
    for ( t = timers; t != NULL; t = next ) {
	next = t->next;
	/* Since the list is sorted, as soon as we find a timer
	** that isn't ready yet, we are done.
	*/
	if ( t->time.tv_sec > now.tv_sec ||
	     ( t->time.tv_sec == now.tv_sec &&
	       t->time.tv_usec > now.tv_usec ) )
	    break;
	(t->timer_proc)( t->client_data, &now );
	if ( t->periodic ) {
	    /* Reschedule. */
	    add_usecs( &t->time, t->usecs );
	    list_resort( t );
	} else
	    timer_cancel( t );
    }
}

void timer_reset( struct timeval* time_now, Timer* t )
{
    struct timeval now;

    getnow( time_now, &now );
    t->time = now;
    add_usecs( &t->time, t->usecs );
    list_resort( t );
}

void timer_cancel( Timer* t )
{
    /* Remove it from the active list. */
    list_remove( t );
    /* And put it on the free list. */
    t->next = free_timers;
    free_timers = t;
    t->prev = NULL;
}

void timer_cleanup( void )
{
    Timer* t;

    while ( free_timers != NULL ) {
	t = free_timers;
	free_timers = t->next;
	free( (void*) t );
    }
}

void timer_destroy( void )
{
    while ( timers != NULL )
	timer_cancel( timers );
    timer_cleanup();
}
