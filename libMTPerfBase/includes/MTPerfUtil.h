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
 
#ifndef MT_PERF_UTIL_H
#define MT_PERF_UTIL_H

#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

#define MAX_ADDR_LEN 64
#define  MAX_CRC_LEN 64
#define MAX_SEQ_NUM  256

typedef struct Perf_Client{
    char     mIpAddr[MAX_ADDR_LEN];
    uint32_t mPort;
} PerfClient;

typedef struct Perf_Nal{
    char     mNalCRC[MAX_CRC_LEN];
    uint32_t mSeq;
} PerfNal;

class MTPerfUtil
{
public:
    virtual ~MTPerfUtil();
	static MTPerfUtil* getInstance();
	int addPerfNal(uint8_t interval, uint32_t seq, const char* content, bool src_perf);
	int addPerfClient(PerfClient* client);
	int flush();
	int dumpServerPerf();
	int dumpClientPerf();

protected:
private:
	MTPerfUtil();
	static MTPerfUtil *mInstance;
	vector<PerfClient*> mPerfClient;
	vector<PerfNal*>    mPerfServerNalOrder;
	vector<PerfNal*>    mPerfClientNalOrder;
};

#endif // MT_SEQ_VERCTOR
