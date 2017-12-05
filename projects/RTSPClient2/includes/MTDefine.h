/*
 @header     : MTPerfRTSP
 @abstract   : an implemention of RTSPClient for performance test
 @discussion : basic type define
 @copyright  : Copyright 2017, Martin.Cheng<martin.cheng@rock-chips.com>. All rights reserved.
 */

#include <errno.h>

#ifndef MT_RTSP_DEFINE_H
#define MT_RTSP_DEFINE_H

typedef  unsigned char      boolean;     /* Boolean */
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
typedef  unsigned char      uint8;       /* Unsigned 08 bit value */
typedef    signed long int  int32;       /*   Signed 32 bit value */
typedef    signed short     int16;       /*   Signed 16 bit value */
typedef    signed char      int8;        /*   Signed 08 bit value */

/* ansi colors */
#define ANSI_BOLD "\033[1m"
#define ANSI_CYAN "\033[36m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_RED "\033[31m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_GREEN "\033[32m"
#define ANSI_WHITE "\033[37m"
#define ANSI_RESET "\033[0m"

/* request/response debug colors */
#define RES_HEADER  ANSI_BOLD ANSI_GREEN
#define REQ_HEADER  ANSI_BOLD ANSI_YELLOW

/* Debug macros */
#define DEBUG_REQ(...)  if (opt_verbose) {                              \
    printf(__VA_ARGS__); }

#define DEBUG_RES(...)  if (opt_verbose) {                              \
    printf(__VA_ARGS__); }

#define RTSP_INFO(...) printf("[RTSP] "); printf(__VA_ARGS__);
#define RTP_INFO(...)  printf("[RTP ] "); printf(__VA_ARGS__);

/* Check if a bit is 1 or 0 */
#define CHECK_BIT(var, pos) !!((var) & (1 << (pos)))

/* Error debug */
#define ERR()     str_error(errno, __FILE__, __LINE__, __FUNCTION__)

/* fcntl pipe value */
#ifndef F_LINUX_SPECIFIC_BASE
#define F_LINUX_SPECIFIC_BASE       1024
#endif
#ifndef F_SETPIPE_SZ
#define F_SETPIPE_SZ	(F_LINUX_SPECIFIC_BASE + 7)
#endif
#ifndef F_GETPIPE_SZ
#define F_GETPIPE_SZ	(F_LINUX_SPECIFIC_BASE + 8)
#endif

void str_error(int errnum, const char *file, int line, const char *func);
long int thread_tid();

#endif /*MT_RTSP_DEFINE_H*/
