# MTStreamPerf

## Summary
Network factors of streaming media are hard to quantify.*MTStreamPerf* is a bandwidth measurement tools for *stream media* over TCP/UDP/HTTP/RTSP. *MTStreamPerf* supports various parameters for precise controls. For each test it reports the measured throughput/bitrate, loss, and other parameters. *MTStreamPerf* aims to quantify the quality of incoming streaming data. 

## Architecture for MTStreamPerf

Client | + | Server
:---:|:------:|:---:
I | --StreamPerf Testing(UDP/TCP/HTTP/RTSP)--> |I
I |<--Data Transfer--|I
I | --QOS Data-->| I
I |<--QOS Report--|I

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
- [ ] documentions
- [ ] support forward error correction(FEC) in rtsp server/client
- [ ] improve the stability
