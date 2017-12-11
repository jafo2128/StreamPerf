# Streaming Task Perf over RTSP(Real Time Streaming Protocol)

## RTSP Features
* RTSP defines control sequences useful in controlling multimedia playback.
* RTSP control channel uses underlying protocols such as RTP/RTCP/RSVP protocols.
* RTSP uses RTP/RTCP/RSVP allows for the implementation of rate adaptation.
* RTSP Data channel uses TCP or UDP protocols.
* Default RTSP port number is 554[3] for both TCP and UDP

## RTSP data-control stream separation
* Client->Server(CSeq=1)  Request: OPTIONS
* Server->Client(CSeq=1) Response: OPTIONS DESCRIBE/SETUP/PLAY/PAUSE/TEARDOWN
* Client->Server(CSeq=2)  Request: DESCRIBE
* Server->Client(CSeq=2) Response: typically in Session Description Protocol (SDP) format
* Client->Server(CSeq=3)  Request: SETUP Transport: RTP/AVP;unicast;client_port=8000-8001
* Server->Client(CSeq=3) Response: Transport: RTP/AVP;unicast;
                                              client_port=8000-8001;
                                              server_port=9000-9001;ssrc=1234ABCD;
                                              Session: 12345678
* Client->Server(CSeq=4)  Request: PLAY rtsp://xxxx.ts RTSP/1.0; Session: 12345678
* Server->Client(CSeq=4) Response: RTP-Info: url=rtsp://xxxx.ts;seq=9810092;rtptime=3450012;Session: 12345678
* Client->Server(CSeq=5)  Request: PAUSE rtsp://xxxx.ts RTSP/1.0; Session: 12345678
* Server->Client(CSeq=5) Response: Session: 12345678
* Client->Server(CSeq=6)  Request: RECORD rtsp://xxxx.ts RTSP/1.0; Session: 12345678
* Server->Client(CSeq=6) Response: Session: 12345678
* Client->Server(CSeq=7)  Request: ANNOUNCE rtsp://xxxx_new.ts RTSP/1.0;
* Server->Client(CSeq=7) Response: ....
* Client->Server(CSeq=8)  Request: TEARDOWN rtsp://xxxx.ts RTSP/1.0; Session: 12345678
* Server->Client(CSeq=8) Response: Session: 12345678

## References: TCP/IP Basic Knowledge
* [Transmission Control Protocol(TCP)](https://en.wikipedia.org/wiki/Transmission_Control_Protocol)  
* [Transmission Control Protocol(TCP-RFC 793)](https://tools.ietf.org/html/rfc793)  
* [User Datagram Protocol(UDP)](https://en.wikipedia.org/wiki/User_Datagram_Protocol)  
* [User Datagram Protocol(UDP)-RFC 768)](https://tools.ietf.org/html/rfc768)  
* [Real Time Streaming Protocol(RTSP)](https://en.wikipedia.org/wiki/Real_Time_Streaming_Protocol)  
* [Real-time Transport Protocol(RTP)](https://en.wikipedia.org/wiki/Real-time_Transport_Protocol)  
* [RTP Control Protocol(RCTP)](https://en.wikipedia.org/wiki/RTP_Control_Protocol) 
* [Hypertext Transfer Protocol(HTTP)](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol)
* [Hypertext Transfer Protocol(HTTP-RFC2068)](https://tools.ietf.org/html/rfc2068)