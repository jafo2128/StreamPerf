#ifndef MTPERFTASK_H_INCLUDED
#define MTPERFTASK_H_INCLUDED

#include "MTTask.h"
#ifdef WIN32
    #include <Winsock2.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #define closesocket close
#endif

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

class MTPerfTask : public MTTask {
public:
    uint32_t mPort;
    char     mUri[1024];
    uint32_t mPktSeq;
public:
    MTPerfTask();
    ~MTPerfTask();
    void initTaskParams(uint32_t port, const char* media_uri);
    int  selectSock(int sock, bool read, bool write, uint32_t timeout_ms);
    int  recordNalCRC(const char* rawCRC);
    int  calcNalCRC(const char* nal, char* crc);
    virtual int doTask(void* args);

};

class MTInsightTaskServer : public MTPerfTask {
public:
    int doTask(void* args);
};

class MTInsightTaskClient : public MTPerfTask {
public:
    int doTask(void* args);
};

class MTMediaTCPTaskServer : public MTPerfTask {
public:
    int doTask(void* args);
};

class MTMediaTCPTaskClient : public MTPerfTask {
public:
    int doTask(void* args);
};

class MTMediaUDPTaskServer : public MTPerfTask {
public:
    int doTask(void* args);
};

class MTMediaUDPTaskClient : public MTPerfTask {
public:
    int doTask(void* args);
};

#endif //MTPERFTASK_H_INCLUDED
