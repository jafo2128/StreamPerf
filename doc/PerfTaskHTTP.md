# Streaming Task Perf over HTTP(Hypertext Transfer Protocol)

## HTTP Features
* HTTP is an application-level protocol for distributed, collaborative, hypermedia information systems. 
* HTTP is a generic, stateless, object-oriented protocol which can be used for many tasks.
* HTTP is the typing and negotiation of data representation
* HTTP allows systems to be built independently of the data being transferred.

## HTTP Client/Server Model(resemble TCP)
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
                         Fig1: TCP Client/Server Model(resemble TCP)

## Request Header Fields
The request-header fields allow the client to pass additional information about the request, and about the client itself, to the server. 
          request-header = Accept                   ; Section 14.1
                         | Accept-Charset           ; Section 14.2
                         | Accept-Encoding          ; Section 14.3
                         | Accept-Language          ; Section 14.4
                         | Authorization            ; Section 14.8
                         | From                     ; Section 14.22
                         | Host                     ; Section 14.23
                         | If-Modified-Since        ; Section 14.24
                         | If-Match                 ; Section 14.25
                         | If-None-Match            ; Section 14.26
                         | If-Range                 ; Section 14.27
                         | If-Unmodified-Since      ; Section 14.28
                         | Max-Forwards             ; Section 14.31
                         | Proxy-Authorization      ; Section 14.34
                         | Range                    ; Section 14.36
                         | Referer                  ; Section 14.37
                         | User-Agent               ; Section 14.42

## Status Code and Reason Phrase
* The Status-Code element is a 3-digit integer result code of the attempt to understand and satisfy the request.
* The Reason-Phrase is intended to give a short textual description of the Status-Code. 
* The Status-Code is intended for use by automata and the Reason-Phrase is intended for the human user.
* The client is not required to examine or display the Reason-Phrase.

The first digit of the Status-Code defines the class of response. The last two digits do not have any categorization role. There are 5 values for the first digit:
 o  1xx: Informational - Request received, continuing process
 o  2xx: Success - The action was successfully received, understood,
    and accepted
 o  3xx: Redirection - Further action must be taken in order to
    complete the request
 o  4xx: Client Error - The request contains bad syntax or cannot be
    fulfilled
 o  5xx: Server Error - The server failed to fulfill an apparently
    valid request
 
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