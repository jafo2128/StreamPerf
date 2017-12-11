# MTStreamPerf

## Summary
Network factors of streaming media are hard to quantify.*MTStreamPerf* is a bandwidth measurement tools for *stream media* over TCP/UDP/HTTP/RTSP. *MTStreamPerf* supports various parameters for precise controls. For each test it reports the measured throughput/bitrate, loss, and other parameters. *MTStreamPerf* aims to quantify the quality of incoming streaming data. 

**MTStreamPerf Features**
* [☆] Supports ThreadPool/ThreadTask
* [☆] Supports Forward Error Correction(WAIT to develop, donate codes?)
* [☆] Supports Streaming Performance Testing over TCP/UDP (will done soon!)
* [☆] Supports Streaming Performance Testing over RTSP (will done soon!)
* [☆] Supports Streaming Performance Testing over HTTP (WAIT to develop, may be abandoned,sorry!)

## MTStreamPerf workflow
``` 
--------------------------------      --------------------------------
   create client thread_task             create server thread_task
--------------------------------      --------------------------------
 init MTTaskSetting/MTTaskStats         init MTTaskSetting/MTTaskStats
--------------------------------      --------------------------------
 add to task_runner(client_task)        add to task_runner(server_task)
--------------------------------      --------------------------------
 Transmission efficiency test   <----> Transmission efficiency test
--------------------------------      --------------------------------
 Error/Exception/Perf statistics<----> Error/Exception/Perf statistics
--------------------------------      --------------------------------
        close task_runner                     close task_runner
--------------------------------      --------------------------------
                         Fig1: MTStreamPerf workflow
```

## MTStreamPerf Framework
MTTask <-- MTPerfTask <-- MTPerfTaskUDP  <-- MTPerfTaskUDPServer  
MTTask <-- MTPerfTask <-- MTPerfTaskUDP  <-- MTPerfTaskUDPClient  
MTTask <-- MTPerfTask <-- MTPerfTaskTCP  <-- MTPerfTaskTCPServer  
MTTask <-- MTPerfTask <-- MTPerfTaskTCP  <-- MTPerfTaskTCPClient  
MTTask <-- MTPerfTask <-- MTPerfTaskRTSP <-- MTPerfTaskRTSPClient(Server is Live555)  
MTTask <-- MTPerfTask <-- MTPerfTaskHTTP <-- MTPerfTaskHTTPClient(Server is Apache)  

## Methods of Compiling
- [ ] comming soon

## MTStreamPerf Documents
- [x] [Understanding MTStreamPerf workflow](doc/README.md)
- [x] [Understanding MTPerfTaskTCP workflow](doc/PerfTaskTCP.md)
- [x] [Understanding MTPerfTaskUDP workflow](doc/PerfTaskUDP.md)
- [x] [Understanding MTPerfTaskRTSP workflow](doc/PerfTaskRTSP.md)
- [x] [Understanding MTPerfTaskHTTP workflow](doc/PerfTaskHTTP.md)

## Third-party Libraries  Dependence
- [x] pthread
- [x] live555(server)

## To do List
- [x] documentions
- [x] MTPerfTaskUDP
- [ ] MTPerfTaskTCP
- [ ] MTPerfTaskRTSP
- [ ] support forward error correction(FEC) in rtsp server/client
- [ ] improve the stability
