#include <iostream>
#include <stdio.h>
#include "Define.h"
#include "MTPerfTaskUDP.h"
using namespace std;

#define TAG "MTStreamPerf"

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
    AndroidLogListener *listener = new AndroidLogListener();
    MTLog::setLogListener(listener);

    MTTaskRunner* scheduler = new MTTaskRunner(5/*max_threads*/, 30/*max_tasks*/);
    scheduler->start();

    //MTPerfTask*  taskHTTP = new MTPerfTaskHTTP();
    //MTPerfTask*  taskRTSP = new MTPerfTaskRTSP();
    //MTPerfTask*  taskTCP = new MTPerfTaskTCP();
    MTPerfTask*  server_udp = new MTPerfTaskUDPServer();
    MTPerfTask*  client_udp = new MTPerfTaskUDPClient();
    //scheduler->addTask(PRIORITY_FIFO, taskHTTP);
    //scheduler->addTask(PRIORITY_FIFO, taskRTSP);
    //scheduler->addTask(PRIORITY_FIFO, taskTCP);
    scheduler->addTask(PRIORITY_FIFO, server_udp);
    scheduler->addTask(PRIORITY_FIFO, client_udp);

    usleep(2000*1000);
    delete scheduler;
    scheduler = NULL;
    //MTPerfUtil::getInstance()->dumpServerPerf();
    //MTPerfUtil::getInstance()->dumpClientPerf();
    //delete MTPerfUtil::getInstance();
    return 0;
}
