# MTStreamPerf

## About MTStreamPerf
Network factors of streaming media are hard to quantify.The first step in streaming media is to analyze the quality of stream.MTStreamPerf aims to quantify the quality of incoming streaming data. MTStreamPerf supports some network protocols such as TCP/UDP/HTTP and RTSP.

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
