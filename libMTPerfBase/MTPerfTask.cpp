#include "Define.h"
#include "MTPerfTask.h"

#define TAG "MTPerfTask"

#define MAX_BUFFER_SIZE 512
#define MAX_SLEEP_TIME  100 /*us*/

/**
 ** Class MTPerfTask
 **/
MTPerfTask::MTPerfTask():MTTask(){
    mPort   = 0;
    mPktSeq = 0;
    memset(mUri, 0, 1024);
}

MTPerfTask::~MTPerfTask(){
}

void MTPerfTask::initTaskParams(uint32_t port, const char* media_uri) {
    mPort  =  port;
    snprintf(mUri, 1024, "%s", media_uri);
}

int MTPerfTask::selectSock(int sock, bool read, bool write, uint32_t timeout_ms) {
    int err = 0;
    fd_set fds_read, fds_writable;
#ifdef _WIN32
    TIMEVAL tv;
#else
    struct timeval tv;
#endif   /* _WIN32 */
    FD_ZERO(&fds_read);
    FD_ZERO(&fds_writable);
    if (read)
        FD_SET(sock, &fds_read);
    if (write)
        FD_SET(sock, &fds_writable);
    timeout_ms = (timeout_ms <  500)? 500: timeout_ms;
    tv.tv_sec  = (timeout_ms / 1000);
    tv.tv_usec = (timeout_ms % 1000)*1000;

    /* select: -1 socket error; 0 timeout; */
    err = select( sock + 1, &fds_read, &fds_writable, NULL, &tv );
    switch(err) {
        case 0:
            err = SOCK_READY_BUSY;
            break;
        case -1:
            if(errno == EINTR) err = SOCK_READY_ERR;
            break;
        default:
            if (read && FD_ISSET(sock, &fds_read))
                err = SOCK_READY_READ;
            if (write && FD_ISSET(sock, &fds_writable))
                err = SOCK_READY_WRITE;
            if (read && write && FD_ISSET(sock, &fds_writable) && FD_ISSET(sock, &fds_read))
                err = SOCK_READY_ALL;
            break;
     }
     return err;
}

int MTPerfTask::doTask(void* args) {
    MTLog::LogEx(TAG, __FUNCTION__, "MTPerfTask(%p) doTask...port(%d) uri(%s)",
                                     this, mPort, mUri);
    return PERF_ERROR_NONE;
}

/**
 ** Class MTInsightTaskServer --- PerfInsightServer over TCP
 **/
int MTInsightTaskServer::doTask(void* args) {
    MTLog::LogEx(TAG, __FUNCTION__, "Insight Server(%p) doTask...port(%d) uri(%s)",
                                     this, mPort, mUri);
    return PERF_ERROR_NONE;
}

/**
 ** Class MTInsightTaskClient --- PerfInsightServer over TCP
 **/
int MTInsightTaskClient::doTask(void* args) {
    MTLog::LogEx(TAG, __FUNCTION__, "Insight Client(%p) doTask...port(%d) uri(%s)",
                                     this, mPort, mUri);
    return PERF_ERROR_NONE;
}

/**
 ** Class MTMediaTCPTaskServer --- VideoStreamSever over TCP
 **/
int MTMediaTCPTaskServer::doTask(void* args) {
    MTLog::LogEx(TAG, __FUNCTION__, "TCP Server(%p) doTask...port(%d) uri(%s)",
                                     this, mPort, mUri);
    return PERF_ERROR_NONE;
}

/**
 ** Class MTMediaTCPTaskClient --- VideoStreamSever over TCP
 **/
int MTMediaTCPTaskClient::doTask(void* args) {
    MTLog::LogEx(TAG, __FUNCTION__, "TCP Client(%p) doTask...port(%d) uri(%s)",
                                     this, mPort, mUri);
    return PERF_ERROR_NONE;
}

/**
 ** Class MTMediaUDPTaskServer --- VideoStreamSever over UDP
 **/
int MTMediaUDPTaskServer::doTask(void* args) {
    int    sock_server = 0;
    int    data_len    = 0;
    int    err         = PERF_ERROR_NONE;
    int    invalid_cnt = 0;
    char   buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in addr_server;
    struct sockaddr_in addr_client;
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(mPort);
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);

#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif  /*  WIN32  */

    if ( (sock_server = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        MTLog::LogEx(TAG, __FUNCTION__, "UDP Server(%p) Fail to socket(SOCK_DGRAM) on port(%d)",
                                     this, mPort);
        sock_server = 0;
        err = PERF_ERROR_SOCK;
        goto TAG_ERROR;
    }
    if ( bind(sock_server, (struct sockaddr *)&addr_server, sizeof(addr_server)) < 0) {
        MTLog::LogEx(TAG, __FUNCTION__, "UDP Server(%p) Fail to bind(SOCK_DGRAM) on port(%d)",
                                     this, mPort);
        err = PERF_ERROR_BIND;
        goto TAG_ERROR;
    }

    mPktSeq  = 0; mPktSeq++;
    data_len = sizeof(addr_client);
    addr_client.sin_family = AF_INET;
    addr_client.sin_port = htons(mPort-10);
    addr_client.sin_addr.s_addr = inet_addr(mUri);
    //data_len = recvfrom(sock_server, buffer, MAX_BUFFER_SIZE-1, 0, (struct sockaddr*)&addr_client, &data_len);

    while ((!mCancel) && (data_len > 0) && (mPktSeq<MAX_SEQ_NUM)) {
        err = selectSock(sock_server, false/*read*/, true/*write*/, 600/*timeout-ms*/);
        switch(err){
            case SOCK_READY_WRITE:
                memset(buffer, 0, MAX_BUFFER_SIZE);
                snprintf(buffer, MAX_BUFFER_SIZE, "%06d device[%p] What do you like?", mPktSeq, this);

                if( sendto(sock_server, buffer, strlen(buffer), 0, (struct sockaddr *)&addr_client, sizeof(addr_client)) < 0){
                    MTLog::LogEx(TAG, __FUNCTION__, "UDP Server(%p) died, Fail to sendto", this);
                    err = PERF_ERROR_TRANS;
                    goto TAG_ERROR;
                }else{
                    MTPerfUtil::getInstance()->addPerfNal(2, mPktSeq, buffer, true/*src_perf--I'm server*/);
                    invalid_cnt = 0;
                    mPktSeq++;
                }

                break;
            case SOCK_READY_ERR:
                goto TAG_ERROR;
                break;
            default:
                invalid_cnt++;
                if(invalid_cnt>5){
                    goto TAG_ERROR;
                    break;
                }
                break;
        }
        usleep(MAX_SLEEP_TIME);
    }
    MTLog::LogEx(TAG, __FUNCTION__, "UDP Server(%p) Task Done(%d)", this);

TAG_ERROR:
    mCancel = true;
    if(sock_server>0) {
        close(sock_server);
    }
    #ifdef WIN32
        WSACleanup();
    #endif
    return err;
}

/**
 ** Class MTMediaUDPTaskClient --- VideoStreamSever over TCP
 **/
int MTMediaUDPTaskClient::doTask(void* args) {
    int    sock_client = 0;
    int    data_len    = 0;
    int    err         = PERF_ERROR_NONE;
    char   buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in addr_client;
    struct sockaddr_in addr_server;
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif  /*  WIN32  */

    if ((sock_client = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        MTLog::LogEx(TAG, __FUNCTION__, "UDP Client(%x) Fail to socket(SOCK_DGRAM) on port(%d)",
                                     this, mPort);
        sock_client = 0;
        err = PERF_ERROR_SOCK;
        goto TAG_ERROR;
    }
    addr_client.sin_family = AF_INET;
    addr_client.sin_port = htons(mPort - 10);
    addr_client.sin_addr.s_addr = htonl(INADDR_ANY);
    if (addr_client.sin_addr.s_addr == INADDR_NONE) {
        MTLog::LogEx(TAG, __FUNCTION__, "UDP Client(%x) Fail to sockaddr_in, target server: %s:%d",
                                         this, mUri, mPort);
        err = PERF_ERROR_ADDR;
        goto TAG_ERROR;
    }

    if( bind(sock_client, (struct sockaddr*)&addr_client, sizeof(addr_client)) < 0) {
        MTLog::LogEx(TAG, __FUNCTION__, "UDP Client(%x) Fail to bind, target server: %s:%d",
                                         this, mUri, mPort);
        err = PERF_ERROR_BIND;
        goto TAG_ERROR;
    }

    //Specify Traget Server(Optional)
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(mPort);
    addr_server.sin_addr.s_addr = inet_addr(mUri);
    if((connect(sock_client,(struct sockaddr*)&addr_server,sizeof(struct sockaddr_in))) < 0){
        MTLog::LogEx(TAG, __FUNCTION__, "UDP Client(%x) Fail to connect, target server: %s:%d",
                                         this, mUri, mPort);
        err = PERF_ERROR_BIND;
        goto TAG_ERROR;
    }

    snprintf(buffer, MAX_BUFFER_SIZE-1, "%s", "udp transport");
    //data_len = sendto(sock_client, buffer, strlen(buffer), 0, (struct sockaddr *)&addr_server, sizeof(addr_server));
    data_len = sizeof(addr_client);
    while (!mCancel && (mPktSeq<(MAX_SEQ_NUM-100))) {
        err = selectSock(sock_client, true/*read*/, false/*write*/, 600/*timeout-ms*/);
        switch(err){
            case SOCK_READY_READ:
                memset(buffer, 0, MAX_BUFFER_SIZE);
                if( recvfrom(sock_client, buffer, MAX_BUFFER_SIZE-1, 0, (struct sockaddr*)&addr_server, &data_len) < 0){
                    MTLog::LogEx(TAG, __FUNCTION__, "UDP Client(%x) died, Fail to recvfrom", this);
                    err = PERF_ERROR_TRANS;
                    goto TAG_ERROR;
                } else {
                    char seq_buf[8] = {""};
                    strncpy(seq_buf, buffer, 6);
                    mPktSeq = atoi(seq_buf);
                    MTPerfUtil::getInstance()->addPerfNal(2, mPktSeq, buffer, false/*src_perf : I'm client*/);
                    MTLog::LogEx(TAG, __FUNCTION__, "UDP Client(%s:%d) Seq=%d Says:%s",
                                 inet_ntoa(addr_server.sin_addr), ntohs(addr_server.sin_port), mPktSeq, buffer);
                }
                break;
            case SOCK_READY_ERR:
                goto TAG_ERROR;
                break;
            default:
                break;
        }
        usleep(MAX_SLEEP_TIME);
    }
    MTLog::LogEx(TAG, __FUNCTION__, "UDP Client(%x) task done....", this);
TAG_ERROR:
    mCancel = true;
    if(sock_client>0) {
        close(sock_client);
    }
    #ifdef WIN32
        WSACleanup();
    #endif
    return err;
}
