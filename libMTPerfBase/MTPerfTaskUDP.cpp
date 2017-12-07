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


#include "MTPerfTaskUDP.h"

#include "Define.h"
#include "MTPerf.h"

#define TAG "MTPerfTaskUDP"

int  MTPerfTaskUDP::netInit() {
    return -1;
}
int  MTPerfTaskUDP::netListen() {
    return -1;
}
int  MTPerfTaskUDP::netAccept() {
    return -1;
}
int  MTPerfTaskUDP::netConnect() {
    return -1;
}
int  MTPerfTaskUDP::netSend() {
    return -1;
}
int  MTPerfTaskUDP::netRecv() {
    return -1;
}

/**
 ** Class MTPerfTaskUDP(Server)
 **/
int MTPerfTaskUDP::doTask(void* args) {
    int    sock_server = 0;
    int    data_len    = 0;
    int    err         = PERF_ERROR_NONE;
    int    invalid_cnt = 0;
    char   buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in addr_server;
    struct sockaddr_in addr_client;
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(mTaskSettings->server_port);
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);

#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif  /*  WIN32  */

    if ( (sock_server = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        MTLog::LogEx(TAG, __FUNCTION__, "UDP Server(%p) Fail to socket(SOCK_DGRAM) on port(%d)",
                                     this, mTaskSettings->server_port);
        sock_server = 0;
        err = PERF_ERROR_SOCK;
        goto TAG_ERROR;
    }
    if ( bind(sock_server, (struct sockaddr *)&addr_server, sizeof(addr_server)) < 0) {
        MTLog::LogEx(TAG, __FUNCTION__, "UDP Server(%p) Fail to bind(SOCK_DGRAM) on port(%d)",
                                     this, mTaskSettings->server_port);
        err = PERF_ERROR_BIND;
        goto TAG_ERROR;
    }

    mTaskStats->blocks = 0;
    data_len = sizeof(addr_client);
    addr_client.sin_family = AF_INET;
    addr_client.sin_port = htons(mTaskSettings->server_port-10);
    addr_client.sin_addr.s_addr = inet_addr(mTaskSettings->server_hostname);
    //data_len = recvfrom(sock_server, buffer, MAX_BUFFER_SIZE-1, 0, (struct sockaddr*)&addr_client, &data_len);

    while ((!mCancel) && (data_len > 0) && (mTaskStats->blocks < MAX_SEQ_NUM)) {
        //err = selectSock(sock_server, false/*read*/, true/*write*/, 600/*timeout-ms*/);
        switch(err){
            case SOCK_READY_WRITE:
                memset(buffer, 0, MAX_BUFFER_SIZE);
                snprintf(buffer, MAX_BUFFER_SIZE, "%s device[%p] What do you like?", mTaskSettings->name, this);

                if( sendto(sock_server, buffer, strlen(buffer), 0, (struct sockaddr *)&addr_client, sizeof(addr_client)) < 0){
                    MTLog::LogEx(TAG, __FUNCTION__, "UDP Server(%p) died, Fail to sendto", this);
                    err = PERF_ERROR_TRANS;
                    goto TAG_ERROR;
                }else{
                    MTPerfUtil::getInstance()->addPerfNal(2, mTaskStats->blocks, buffer, true/*src_perf--I'm server*/);
                    invalid_cnt = 0;
                    mTaskStats->blocks++;
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
 ** Class MTPerfTaskUDP(Client)
 **/
int MTPerfTaskUDP::doTask2(void* args) {
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
                                     this, mTaskSettings->server_port);
        sock_client = 0;
        err = PERF_ERROR_SOCK;
        goto TAG_ERROR;
    }
    addr_client.sin_family = AF_INET;
    addr_client.sin_port = htons(mTaskSettings->server_port - 10);
    addr_client.sin_addr.s_addr = htonl(INADDR_ANY);
    if (addr_client.sin_addr.s_addr == INADDR_NONE) {
        MTLog::LogEx(TAG, __FUNCTION__, "UDP Client(%x) Fail to sockaddr_in, target server: %s:%d",
                                         this, mTaskSettings->server_hostname, mTaskSettings->server_port);
        err = PERF_ERROR_ADDR;
        goto TAG_ERROR;
    }

    if( bind(sock_client, (struct sockaddr*)&addr_client, sizeof(addr_client)) < 0) {
        MTLog::LogEx(TAG, __FUNCTION__, "UDP Client(%x) Fail to bind, target server: %s:%d",
                                         this, mTaskSettings->server_hostname, mTaskSettings->server_port);
        err = PERF_ERROR_BIND;
        goto TAG_ERROR;
    }

    //Specify Traget Server(Optional)
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(mTaskSettings->server_port);
    addr_server.sin_addr.s_addr = inet_addr(mTaskSettings->server_hostname);
    if((connect(sock_client,(struct sockaddr*)&addr_server,sizeof(struct sockaddr_in))) < 0){
        MTLog::LogEx(TAG, __FUNCTION__, "UDP Client(%x) Fail to connect, target server: %s:%d",
                                         this, mTaskSettings->server_hostname, mTaskSettings->server_port);
        err = PERF_ERROR_BIND;
        goto TAG_ERROR;
    }

    snprintf(buffer, MAX_BUFFER_SIZE-1, "%s", "udp transport");
    //data_len = sendto(sock_client, buffer, strlen(buffer), 0, (struct sockaddr *)&addr_server, sizeof(addr_server));
    data_len = sizeof(addr_client);
    while (!mCancel && (mTaskStats->blocks <(MAX_SEQ_NUM-100))) {
        //err = selectSock(sock_client, true/*read*/, false/*write*/, 600/*timeout-ms*/);
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
                    mTaskStats->blocks = atoi(seq_buf);
                    MTPerfUtil::getInstance()->addPerfNal(2, mTaskStats->blocks, buffer, false/*src_perf : I'm client*/);
                    MTLog::LogEx(TAG, __FUNCTION__, "UDP Client(%s:%d) Seq=%d Says:%s",
                                 inet_ntoa(addr_server.sin_addr), ntohs(addr_server.sin_port), mTaskStats->blocks, buffer);
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
