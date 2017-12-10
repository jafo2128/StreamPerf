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
    mTaskStats    = NULL;
    mTsBuffer     = NULL;
    mTagName      = NULL;
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
    mTaskSettings->sock_domain   = AF_UNSPEC;
    mTaskSettings->sock_type     = 0;    /* unknown*/
    mTaskSettings->omit          = DEFAULT_OMIT;
    mTaskSettings->sock_bufsize  = 0;    /* use autotuning */
    mTaskSettings->block_size    = DEFAULT_TCP_BLKSIZE;
    mTaskSettings->remote_port   = DEFAULT_PORT;
    mTaskSettings->local_port    = DEFAULT_PORT;
    mTaskSettings->sock_timeout  = DEFAULT_SOCK_TIMEOUT;

    mTaskSettings->duration      = DEFAULT_DURATION;
    mTaskSettings->diskfile_name = (char*) 0;
    mTaskSettings->title         = NULL;
    mTaskSettings->congestion    = NULL;
    mTaskSettings->congestion_used = NULL;
    mTaskSettings->remote_congestion_used = NULL;
    mTaskSettings->ctrl_sck      = -1;
    mTaskSettings->prot_listener = -1;

    //mTaskSettings->stats_callback = iperf_stats_callback;
    //mTaskSettings->reporter_callback = iperf_reporter_callback;
    mTaskSettings->stats_interval = mTaskSettings->reporter_interval = 1;
    mTaskSettings->multisend      = 10;	/* arbitrary */

    /*init task stats*/
    mTaskStats = (struct task_stats*)malloc(sizeof(struct task_stats));
    memset(mTaskStats, 0, sizeof(struct task_stats));
    mTaskStats->unit_format = 'a';
    mTaskStats->rate = 0;
    mTaskStats->fqrate = 0;
    mTaskStats->pacing_timer = 1000;
    mTaskStats->burst = 0;
    mTaskStats->mss = 0;
    mTaskStats->bytes_recv_interval = 0;
    mTaskStats->bytes_send_interval = 0;
    mTaskStats->bytes_recv  = mTaskStats->bytes_send  = 0;
    mTaskStats->blocks_recv = mTaskStats->blocks_send = 0;
    /*quality of packet*/
    mTaskStats->packet_index        = 0;
    mTaskStats->packet_cnt_outorder = 0;
    mTaskStats->packet_cnt_error    = 0;

    return 0;
}

int MTPerfTask::doTask(void* args) {
    MTLog::LogEx(TAG, __FUNCTION__, "MTPerfTask(%p) doTask(%p)...", this, mTaskSettings);
    return PERF_ERROR_NONE;
}

int  MTPerfTask::recordNalCRC(const char* rawCRC) {
    return PERF_ERROR_NONE;
}
int  MTPerfTask::calcNalCRC(const char* nal, char* crc) {
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
