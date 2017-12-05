# MTStreamPerf

## About MTStreamPerf
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

## MTStreamPerf Usage
- [ ] comming soon

## Third-party Libraries  Dependence
- [x] pthread
- [x] live555(server)

## To do List
- [ ] documentions
- [ ] support fec in rtsp server/client
- [ ] improve the stability
