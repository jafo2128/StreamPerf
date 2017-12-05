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
