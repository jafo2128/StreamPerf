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
 *       original author: shareviews@sina.com (2017-12-XX) without permission
 */
 
#ifndef MTPERFTASK_H_INCLUDED
#define MTPERFTASK_H_INCLUDED

#include "MTTask.h"

typedef enum {
    PERF_ERROR_NONE   = 0,
    PERF_ERROR_SOCK   = 1,
    PERF_ERROR_BIND   = 2,
    PERF_ERROR_ADDR   = 3,
    PERF_ERROR_TRANS  = 4,
} PERF_TASK_ERROR;

typedef enum {
    SOCK_READY_ERR    = 0,
    SOCK_READY_BUSY   = 1,
    SOCK_READY_READ   = 2,
    SOCK_READY_WRITE  = 3,
    SOCK_READY_ALL    = 4,
    SOCK_READY_MAX    = 5,
} SOCK_READY_TYPE;

struct task_settings;
struct task_stats;
class MTPerfTask : public MTTask {
public:
    struct task_settings*  mTaskSettings;
    struct task_stats*     mTaskStats;
public:
    MTPerfTask();
    ~MTPerfTask();
    struct task_settings* getTaskSettings();
    struct task_stats*    getTaskStats();

    virtual int  netInit() = 0;
    virtual int  netListen() = 0;
    virtual int  netAccept() = 0;
    virtual int  netConnect() = 0;
    virtual int  netSend() = 0;
    virtual int  netRecv() = 0;
    virtual int  doTask(void* args);
    virtual int  recordNalCRC(const char* rawCRC);
    virtual int  calcNalCRC(const char* nal, char* crc);

private:
    int initDefaultTaskParams();
};

class MTPerfTaskTCP : public MTPerfTask {
public:
    int doTask(void* args);
};

class MTPerfTaskHTTP : public MTPerfTask {
public:
    int doTask(void* args);
};

class MTPerfTaskRTSP : public MTPerfTask {
public:
    int doTask(void* args);
};

#endif //MTPERFTASK_H_INCLUDED
