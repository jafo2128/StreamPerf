# Streaming Task Perf over UDP(User Datagram Protocol)

## UDP Features
* UDP  provides  a procedure to send data with a minimum  of protocol mechanism.  
* UDP  is transaction oriented, and delivery and duplicate protection are not guaranteed.  
* Applications requiring ordered reliable delivery of streams of data should use the Transmission Control Protocol (TCP) 

## UDP Client/Server Model
--------------------------------      --------------------------------
    socket(s_domain, s_type)              socket(s_domain, s_type)
--------------------------------      --------------------------------
         bind(sock, ...)                      bind(sock, ...)
--------------------------------      --------------------------------
     select() then send/revc    <---->       select() then send/revc
--------------------------------      --------------------------------
         close(sock)                          close(sock)
--------------------------------      --------------------------------
                         Fig1: UDP Client/Server Model

## UDP Header Format
                  0      7 8     15 16    23 24    31
                 +--------+--------+--------+--------+
                 |     Source      |   Destination   |
                 |      Port       |      Port       |
                 +--------+--------+--------+--------+
                 |                 |                 |
                 |     Length      |    Checksum     |
                 +--------+--------+--------+--------+
                 |
                 |          data octets ...
                 +---------------- ...

                      Fig2: User Datagram Header Format


## Relation to Other Protocols

  The following diagram illustrates the place of the TCP in the protocol
  hierarchy:


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