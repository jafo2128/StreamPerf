#MTStreamPerf Document

## MTStreamPerf Features
* [☆] Supports ThreadPool/ThreadTask
* [☆] Supports Forward Error Correction(WAIT to develop, donate codes?)
* [☆] Supports Streaming Performance Testing over TCP/UDP (will done soon!)
* [☆] Supports Streaming Performance Testing over RTSP (will done soon!)
* [☆] Supports Streaming Performance Testing over HTTP (WAIT to develop, may be abandoned,sorry!)

## MTStreamPerf workflow
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

## MTStreamPerf Framework
MTTask <-- MTPerfTask <-- MTPerfTaskUDP  <-- MTPerfTaskUDPServer
MTTask <-- MTPerfTask <-- MTPerfTaskUDP  <-- MTPerfTaskUDPClient
MTTask <-- MTPerfTask <-- MTPerfTaskTCP  <-- MTPerfTaskTCPServer
MTTask <-- MTPerfTask <-- MTPerfTaskTCP  <-- MTPerfTaskTCPClient
MTTask <-- MTPerfTask <-- MTPerfTaskRTSP <-- MTPerfTaskRTSPClient(Server is Live555)
MTTask <-- MTPerfTask <-- MTPerfTaskHTTP <-- MTPerfTaskHTTPClient(Server is Apache)

## MTStreamPerf Documents
- [x] [Understanding MTStreamPerf workflow](README.md)
- [x] [Understanding MTPerfTaskTCP workflow](PerfTaskTCP.md)
- [x] [Understanding MTPerfTaskUDP workflow](PerfTaskUDP.md)
- [x] [Understanding MTPerfTaskRTSP workflow](PerfTaskRTSP.md)
- [x] [Understanding MTPerfTaskHTTP workflow](PerfTaskHTTP.md)
