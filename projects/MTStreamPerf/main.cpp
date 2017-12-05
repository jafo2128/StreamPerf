#include <iostream>
#include <stdio.h>
#include "Define.h"
using namespace std;

#define TAG "StreamServer-Win"

#define PORT_INSIGHT   8989
#define PORT_MEDIA_UDP 8080
#define PORT_MEDIA_TCP 9090

class AndroidLogListener : public MTLogListener{
    virtual void OsLog(const char* tag, int level, const char* message){
       #ifdef WIN32
       printf("%16s: %s\n", tag, message);
       #endif
    }
};

int main(int argc, char *argv[])
{
    const char* server_uri = "192.168.0.105";
    AndroidLogListener *listener = new AndroidLogListener();
    MTLog::setLogListener(listener);

    MTTaskRunner* scheduler = new MTTaskRunner(5/*max_threads*/, 30/*max_tasks*/);
    scheduler->start();

    MTInsightTaskServer*  perfServer = new MTInsightTaskServer();
    MTInsightTaskClient*  perfClient = new MTInsightTaskClient();
    MTMediaUDPTaskServer* udpServer  = new MTMediaUDPTaskServer();
    MTMediaUDPTaskClient* udpClient  = new MTMediaUDPTaskClient();
    perfServer->initTaskParams(PORT_INSIGHT, "");
    perfClient->initTaskParams(PORT_INSIGHT, "");
    udpServer->initTaskParams(PORT_MEDIA_UDP, server_uri);
    udpClient->initTaskParams(PORT_MEDIA_UDP, server_uri);
    scheduler->addTask(PRIORITY_FIFO, perfServer);
    scheduler->addTask(PRIORITY_FIFO, perfClient);
    scheduler->addTask(PRIORITY_FIFO, udpServer);
    scheduler->addTask(PRIORITY_FIFO, udpClient);

    usleep(2000*1000);
    delete scheduler;
    scheduler = NULL;
    MTPerfUtil::getInstance()->dumpServerPerf();
    MTPerfUtil::getInstance()->dumpClientPerf();
    //delete MTPerfUtil::getInstance();
    return 0;
}
