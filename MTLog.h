/*
 *      Copyright (C) 2017-2020 Team MediaTime
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

#pragma once

#include <stdio.h>

//#include "MTSystem/SkOSFile.h"
//#include "MTSystem/SkString.h"

#define LOG_LEVEL_NONE         -1 // nothing at all is logged
#define LOG_LEVEL_NORMAL        0 // shows notice, error, severe and fatal
#define LOG_LEVEL_DEBUG         1 // shows all
#define LOG_LEVEL_DEBUG_FREEMEM 2 // shows all + shows freemem on screen
#define LOG_LEVEL_MAX           LOG_LEVEL_DEBUG_FREEMEM

// ones we use in the code
#define LOGDEBUG   0
#define LOGINFO    1
#define LOGNOTICE  2
#define LOGWARNING 3
#define LOGERROR   4
#define LOGSEVERE  5
#define LOGFATAL   6
#define LOGNONE    7

// extra masks - from bit 5
#define LOGMASKBIT    5
#define LOGMASK       ((1 << LOGMASKBIT) - 1)

#define LOGSAMBA      (1 << (LOGMASKBIT + 0))
#define LOGCURL       (1 << (LOGMASKBIT + 1))
#define LOGFFMPEG     (1 << (LOGMASKBIT + 2))
#define LOGDBUS       (1 << (LOGMASKBIT + 4))
#define LOGJSONRPC    (1 << (LOGMASKBIT + 5))
#define LOGAUDIO      (1 << (LOGMASKBIT + 6))
#define LOGAIRTUNES   (1 << (LOGMASKBIT + 7))
#define LOGUPNP       (1 << (LOGMASKBIT + 8))
#define LOGCEC        (1 << (LOGMASKBIT + 9))
#define LOGVIDEO      (1 << (LOGMASKBIT + 10))
#define LOGWEBSERVER  (1 << (LOGMASKBIT + 11))


/*Log Print Toolkit, virtual class*/
class MTLogListener {
public:
    MTLogListener(){};
    ~MTLogListener(){};
    virtual void OsLog(const char*tag, int level, const char*message) = 0;
};

class MTLog
{
public:
  virtual ~MTLog() {}

  static void Log(int logLevel, const char *format, ...);
  static void LogE(const char* tag, const char *format, ...);
  static void LogD(const char* tag, const char *format, ...);
  static void LogEx(const char* tag, const char *func, const char *format, ...);
  static void SetLogLevel(int logLevel);
  static void setLogListener(MTLogListener* listener){mLogListener = listener;};
private:
  static bool PrintLog(const char* tag, int logLevel, const char* message);
  static bool CheckLogLevel(int logLevel);
private:
  static int            mLogLevel;
  static MTLogListener* mLogListener;
};


