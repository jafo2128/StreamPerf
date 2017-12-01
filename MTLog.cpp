/*
 *      Copyright (C) 2017-2020 MediaTime
 *      http://MediaTime.com
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
 */
#include<stdarg.h>
#include<stdio.h>
#include "MTLog.h"

#define DEFAULT_LOG_PATH "MediaTime.log"
#define MAX_LOG_SIZE 2048

int            MTLog::mLogLevel    = LOG_LEVEL_NORMAL;
MTLogListener* MTLog::mLogListener = NULL;

static const char* const levelNames[] =
{"DEBUG", "INFO", "NOTICE", "WARNING", "ERROR", "SEVERE", "FATAL", "NONE"};

// add 1 to level number to get index of name
static const char* const logLevelNames[] =
{ "LOG_LEVEL_NONE" /*-1*/, "LOG_LEVEL_NORMAL" /*0*/, "LOG_LEVEL_DEBUG" /*1*/, "LOG_LEVEL_DEBUG_FREEMEM" /*2*/ };


void MTLog::Log(int logLevel, const char *format, ...) {
    va_list va;
    va_start(va, format);
    char message[MAX_LOG_SIZE]={0};
    vsnprintf(message,sizeof(message),format,va);
    //SkString message = SkStringPrintf(format, va);
    va_end(va);

//#if defined(_DEBUG) || defined(PROFILE)
    PrintLog(NULL, logLevel, message);
//#endif

}

void MTLog::LogE(const char* tag, const char *format, ...) {
  va_list va;
  va_start(va, format);
  char message[MAX_LOG_SIZE]={0};
  vsnprintf(message,sizeof(message), format, va);
  va_end(va);

  PrintLog(tag, LOGERROR, message);
}

void MTLog::LogD(const char* tag, const char *format, ...) {
  va_list va;
  va_start(va, format);
  char message[MAX_LOG_SIZE]={0};
  vsnprintf(message,sizeof(message), format, va);
  va_end(va);

  PrintLog(tag, LOGDEBUG, message);
}

void MTLog::LogEx(const char* tag, const char *func, const char *format, ...) {
  char format_ex[MAX_LOG_SIZE]={0};
  snprintf(format_ex, MAX_LOG_SIZE, "%16s ==> %s", func, format);

  va_list va;
  va_start(va, format);
  char message[MAX_LOG_SIZE]={0};
  vsnprintf(message,sizeof(message), format_ex, va);
  va_end(va);

  PrintLog(tag, LOGERROR, message);
}

void MTLog::SetLogLevel(int logLevel) {
    if (logLevel >= LOG_LEVEL_NONE && logLevel <= LOG_LEVEL_MAX) {
        mLogLevel = logLevel;
        MTLog::Log(LOGNOTICE, "SetLogLevel logLevel = \"%s\"", logLevelNames[mLogLevel + 1]);
    }
}


bool MTLog::PrintLog(const char* tag, int logLevel, const char* message) {
    if(!CheckLogLevel(logLevel)) {
        return false;
    }
    if(NULL != mLogListener) {
        mLogListener->OsLog(tag, logLevel, message);
    }
    return true;
}

bool MTLog::CheckLogLevel(int logLevel) {
    const int extras = (logLevel & ~LOGMASK);
    if (extras != 0 && (mLogLevel & extras) == 0)
        return false;

    if (mLogLevel >= LOG_LEVEL_DEBUG)
        return true;
    if (mLogLevel <= LOG_LEVEL_NONE)
        return false;

    // "mLogLevel" is "LOG_LEVEL_NORMAL"
    return (logLevel & LOGMASK) >= LOGNOTICE;
}

