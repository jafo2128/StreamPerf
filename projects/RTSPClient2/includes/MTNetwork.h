/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */



#ifndef MT_NETWORK_H
#define MT_NETWORK_H
#pragma once

#include <stdint.h>
#include "MTDefine.h" /*base type*/

/* networking I/O headers*/
#ifdef WIN32
    #include <Winsock2.h>
#else
    #include <syscall.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <sys/un.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #define closesocket close
#endif

int open_net_driver();
int close_net_driver();
int net_tcp_connect(char *host, unsigned long port);
int net_udp_connect(char *host, unsigned long port);
int net_unix_sock(const char *path);
int net_sock_nonblock(int sockfd);
int net_sock_cork(int fd, int state);
int net_send8(int fd, uint8_t n);
int net_send16(int fd, uint16_t n);
int net_send32(int fd, uint32_t n);

#endif /*MT_NETWORK_H*/
