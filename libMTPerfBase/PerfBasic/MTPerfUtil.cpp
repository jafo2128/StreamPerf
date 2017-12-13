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

#include "MTPerfUtil.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "MTBase64.h"

MTPerfUtil* MTPerfUtil::mInstance = NULL;

/*
 * private function
 */
MTPerfUtil::MTPerfUtil(){
    mPerfServerNalOrder.clear();
    mPerfClientNalOrder.clear();
}

MTPerfUtil* MTPerfUtil::getInstance() {
    if(NULL == mInstance) {
        mInstance = new MTPerfUtil();
    }
    return mInstance;
}

MTPerfUtil::~MTPerfUtil() {
    flush();
}

//ascending order
static int cmpPerfNal(PerfNal* nal_a, PerfNal* nal_b){
    return (nal_a->mSeq < nal_b->mSeq);
}

int MTPerfUtil::addPerfNal(uint8_t interval, uint32_t seq, const char* content, bool src_perf) {
    int normal_seq = seq/interval;
    interval = (interval>0)? 2:interval;
    std::string base64_nal = Base64::Encode(content, 30/*length*/);
    if( (normal_seq > 0) && (seq%interval == 0)&& (NULL!=content) && (normal_seq < MAX_SEQ_NUM) ) {
        PerfNal *perfNal = (PerfNal*)malloc(sizeof(PerfNal));
        perfNal->mSeq     = normal_seq;
        snprintf(perfNal->mNalCRC, MAX_CRC_LEN, "%s", base64_nal.c_str());
        if(src_perf) {
            mPerfServerNalOrder.push_back(perfNal);
        } else {
            //iterator lower_bound( const key_type &key ): find first element which key_value >= key
            //iterator upper_bound( const key_type &key ): find first element which key_value  > key
            //ascending order
            vector<PerfNal*> ::iterator it;
            it = std::lower_bound(mPerfClientNalOrder.begin(),mPerfClientNalOrder.end(),perfNal, cmpPerfNal);
            mPerfClientNalOrder.insert(it, perfNal);
        }
    }
    return -1;
}

int MTPerfUtil::flush(){
    vector<PerfNal*>::iterator it;
    for(it=mPerfServerNalOrder.begin();it!=mPerfServerNalOrder.end();){
        PerfNal* nal = *it;
        free(nal); *it = NULL;
        it = mPerfServerNalOrder.erase(it);
    }
    mPerfServerNalOrder.clear();
    for(it=mPerfClientNalOrder.begin();it!=mPerfClientNalOrder.end();){
        PerfNal* nal = *it;
        free(nal); *it = NULL;
        it = mPerfClientNalOrder.erase(it);
    }
    mPerfClientNalOrder.clear();
    return 0;
}

#define BASE64_SERVER_NAME "./media_server.base64"
#define BASE64_CLIENT_NAME "./media_client.base64"
int MTPerfUtil::dumpServerPerf() {
    remove(BASE64_SERVER_NAME);
    FILE* base64 = fopen(BASE64_SERVER_NAME, "w+");
    if(NULL != base64) {
        vector<PerfNal*>::iterator it;
        for(it=mPerfServerNalOrder.begin(); it!=mPerfServerNalOrder.end(); it++){
            PerfNal* nal = *it;
            fprintf(base64, "%s\n", nal->mNalCRC);
        }
        fflush(base64);
        fclose(base64);
        base64 = NULL;
    }
    return 0;
}

int MTPerfUtil::dumpClientPerf() {
    remove(BASE64_CLIENT_NAME);
    FILE* base64 = fopen(BASE64_CLIENT_NAME, "w+");

    if(NULL != base64) {
        vector<PerfNal*>::iterator it;
        for(it=mPerfClientNalOrder.begin(); it!=mPerfClientNalOrder.end(); it++){
            PerfNal* nal = *it;
            fprintf(base64, "%s\n", nal->mNalCRC);
        }
        fflush(base64);
        fclose(base64);
        base64 = NULL;
    }
    return 0;
}
