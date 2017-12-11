# Streaming Task Perf over TCP(Transmission Control Protocol)

## TCP Features
TCP provides reliable, ordered, and error-checked delivery of a stream of octets between applications running on hosts communicating by an IP network. Major Internet applications such as the World Wide Web, email, remote administration, and file transfer rely on TCP. Applications that do not require reliable data stream service may use the User Datagram Protocol (UDP), which provides a connectionless datagram service that emphasizes reduced latency over reliability.

## TCP Client/Server Model
```
--------------------------------      --------------------------------
    socket(s_domain, s_type)              socket(s_domain, s_type)
--------------------------------      --------------------------------
         bind(sock, ...)                      bind(sock, ...)
--------------------------------      --------------------------------
         listen(sock, ...) 
--------------------------------      --------------------------------
        select() then accept()               connect(sock, ...)
--------------------------------      --------------------------------
    select() then send/revc    <---->     select() then send/revc
--------------------------------      --------------------------------
          close(sock)                            close(sock)
--------------------------------      --------------------------------
                         Fig1: TCP Client/Server Model
```

## TCP Header Format
```
    0                   1                   2                   3  
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  
   |          Source Port          |       Destination Port        |  
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  
   |                        Sequence Number                        |  
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  
   |                    Acknowledgment Number                      |  
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  
   |  Data |           |U|A|P|R|S|F|                               |  
   | Offset| Reserved  |R|C|S|S|Y|I|            Window             |  
   |       |           |G|K|H|T|N|N|                               |  
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  
   |           Checksum            |         Urgent Pointer        |  
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  
   |                    Options                    |    Padding    |  
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  
   |                             data                              |  
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  

                            Fig2: TCP Header  Format
```

## Relation to Other Protocols

  The following diagram illustrates the place of the TCP in the protocol
  hierarchy:
 ```
       +------+ +-----+ +-----+       +-----+
       |Telnet| | FTP | |Voice|  ...  |     |  Application Level
       +------+ +-----+ +-----+       +-----+
             |   |         |             |
            +-----+     +-----+       +-----+
            | TCP |     | RTP |  ...  |     |  Host Level
            +-----+     +-----+       +-----+
               |           |             |
            +-------------------------------+
            |    Internet Protocol & ICMP   |  Gateway Level
            +-------------------------------+
                           |
              +---------------------------+
              |   Local Network Protocol  |    Network Level
              +---------------------------+

                         Fig3: Protocol Relationships
```   

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
