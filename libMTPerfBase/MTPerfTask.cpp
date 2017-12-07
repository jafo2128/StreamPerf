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
 *   Original Author: shareviews@sina.com (2017-12-XX)
 */

#include "Define.h"
#include "MTPerf.h"
#include "MTPerfTask.h"

#define TAG "MTPerfTask"

/**
 ** Class MTPerfTask
 **/
MTPerfTask::MTPerfTask():MTTask(){
    mTaskSettings = NULL;
    mTaskStats   = NULL;
    initDefaultTaskParams();
}

MTPerfTask::~MTPerfTask(){
    if(NULL != mTaskSettings) {
        free(mTaskSettings);
        mTaskSettings = NULL;
    }
    if(NULL != mTaskStats) {
        free(mTaskStats);
        mTaskStats = NULL;
    }
}

struct task_settings* MTPerfTask::getTaskSettings(){
    return mTaskSettings;
};

struct task_stats* MTPerfTask::getTaskStats(){
    return mTaskStats;
};

int MTPerfTask::initDefaultTaskParams(){
    /*init task settings*/
    mTaskSettings = (struct task_settings*)malloc(sizeof(struct task_settings));
    memset(mTaskSettings, 0, sizeof(struct task_settings));
    mTaskSettings->omit = DEFAULT_OMIT;
    mTaskSettings->duration = DEFAULT_DURATION;
    mTaskSettings->diskfile_name = (char*) 0;
    mTaskSettings->title = NULL;
    mTaskSettings->congestion = NULL;
    mTaskSettings->congestion_used = NULL;
    mTaskSettings->remote_congestion_used = NULL;
    mTaskSettings->server_port = DEFAULT_PORT;
    mTaskSettings->ctrl_sck = -1;
    mTaskSettings->prot_listener = -1;

    //mTaskSettings->stats_callback = iperf_stats_callback;
    //mTaskSettings->reporter_callback = iperf_reporter_callback;

    mTaskSettings->stats_interval = mTaskSettings->reporter_interval = 1;
    mTaskSettings->multisend      = 10;	/* arbitrary */

    /*init task stats*/
    mTaskStats = (struct task_stats*)malloc(sizeof(struct task_stats));
    memset(mTaskStats, 0, sizeof(struct task_stats));
    mTaskStats->domain = AF_UNSPEC;
    mTaskStats->unit_format = 'a';
    mTaskStats->socket_bufsize = 0;    /* use autotuning */
    mTaskStats->blksize = DEFAULT_TCP_BLKSIZE;
    mTaskStats->rate = 0;
    mTaskStats->fqrate = 0;
    mTaskStats->pacing_timer = 1000;
    mTaskStats->burst = 0;
    mTaskStats->mss = 0;
    mTaskStats->bytes = 0;
    mTaskStats->blocks = 0;
    mTaskStats->connect_timeout = -1;

    return 0;
}

int MTPerfTask::doTask(void* args) {
    MTLog::LogEx(TAG, __FUNCTION__, "MTPerfTask(%p) doTask(%p)...", this, mTaskSettings);
    return PERF_ERROR_NONE;
}

/**
 ** Class MTPerfTaskTCP --- PerfInsightServer over TCP
 **/
int MTPerfTaskTCP::doTask(void* args) {
    MTLog::LogEx(TAG, __FUNCTION__, "MTPerfTaskTCP(%p) doTask(%p)...", this, mTaskSettings);
    return PERF_ERROR_NONE;
}

/**
 ** Class MTPerfTaskHTTP --- VideoStreamSever over HTTP
 **/
int MTPerfTaskHTTP::doTask(void* args) {
    MTLog::LogEx(TAG, __FUNCTION__, "MTPerfTaskHTTP(%p) doTask(%p)...", this, mTaskSettings);
    return PERF_ERROR_NONE;
}

/**
 ** Class MTPerfTaskRTSP --- VideoStreamSever over RTSP
 **/
int MTPerfTaskRTSP::doTask(void* args) {
    MTLog::LogEx(TAG, __FUNCTION__, "MTPerfTaskRTSP(%p) doTask(%p)...", this, mTaskSettings);
    return PERF_ERROR_NONE;
}
