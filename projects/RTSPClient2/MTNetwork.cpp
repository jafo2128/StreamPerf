/*
 * MTPerfRTSP
 * an implemention of RTSPClient for performance test
 * base64 encoder and decoder
 * Copyright 2017, Martin.Cheng<martin.cheng@rock-chips.com>. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

/* generic */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "MTNetwork.h"

const char* TAG = "MTNetwork";

int open_net_driver(){
    WORD wVersion;
    WSADATA wsaData;
    if ( 0 != WSAStartup( MAKEWORD( 1, 1 ), &wsaData )) {
        printf("[net-error] Fail to WSAStartup");
        return -1;
    }
    return 0;
}

int close_net_driver(){
    WSACleanup();
    return 0;
}

/* Connect to a TCP socket server and returns the file descriptor */
int net_tcp_connect(char *host_ip, unsigned long port)
{
    int sock_fd;
    struct sockaddr_in *remote;

    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd <= 0) {
	    printf("[net-error] could not create socket\n");
	    return -1;
    }

    remote = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    remote->sin_family = AF_INET;
    remote->sin_port   = htons(port);
#if 1
    remote->sin_addr.s_addr = inet_addr(host_ip);
#else
    int res = inet_pton(AF_INET, host_ip, (void *) (&(remote->sin_addr.s_addr)));
    if (res < 0) {
	    printf("[net-error] Can't set remote->sin_addr.s_addr\n");
	    free(remote);
	    return -1;
    } else if (res == 0) {
	    printf("[net-error] Invalid address '%s'\n", host_ip);
	    free(remote);
	    return -1;
    }
#endif

    if (connect(sock_fd, (struct sockaddr *) remote, sizeof(struct sockaddr)) == -1) {
        close(sock_fd);
        printf("[net-error] connecting to %s:%lu\n", host_ip, port);
        free(remote);
        return -1;
    }

    free(remote);
    return sock_fd;
}


/* Connect to a UDP socket server and returns the file descriptor */
int net_udp_connect(char *host_ip, unsigned long port)
{
    int sock_fd;
    struct sockaddr_in *remote;

    sock_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd <= 0) {
	    printf("[net-error] could not create socket\n");
	    return -1;
    }

    remote = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    remote->sin_family = AF_INET;
    remote->sin_port = htons(port);
#if 1
    remote->sin_addr.s_addr = inet_addr(host_ip);
#else
    int res = inet_pton(AF_INET, host_ip, (void *) (&(remote->sin_addr.s_addr)));
    if (res < 0) {
	    printf("[net-error] Can't set remote->sin_addr.s_addr\n");
	    free(remote);
	    return -1;
    } else if (res == 0) {
	    printf("[net-error] Invalid address '%s'\n", host_ip);
	    free(remote);
	    return -1;
    }
#endif


    if (connect(sock_fd, (struct sockaddr *) remote, sizeof(struct sockaddr)) == -1) {
        close(sock_fd);
        printf("[net-error] connecting to %s:%lu\n", host_ip, port);
        free(remote);
        return -1;
    }

    free(remote);
    return sock_fd;
}

int net_unix_sock(const char *path)
{
    int server_fd;
    size_t address_length;
    struct sockaddr_in address;

    /* Create listening socket */
    server_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("[net-error] Fail to create socket()");
        exit(EXIT_FAILURE);
    }

    /* just in case, remove previous sock */
#ifdef WIN32
    address_length = sizeof(address);
#else
    unlink(path);
    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, sizeof(address.sun_path), path);
    address_length = sizeof(address.sun_family) + strlen(path);
#endif // WIN32

    if (bind(server_fd, (struct sockaddr *) &address, address_length) != 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) != 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

int net_sock_nonblock(int sockfd)
{
#ifdef WIN32
    unsigned long flag=1;
    ioctlsocket(sockfd,FIONBIO,&flag);
#else
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        perror("fcntl");
        return -1;
    }
#endif
   return 0;
}

/*
 * to improve the utilization efficiency of the network
 * Nagle algorithm avoid network congestion which caused by too many small packets
 * CORK algorithm avoid network congestion is to improve the utilization efficiency of the network,
 * making head takes up the proportion of the overall agreement as small as possible.
 * TCP_NODELAY£ºdisable nangle algorithm, send immediately
 * TCP_CORK: Merge small packets into big packets, then
 * int setsockopt(SOCKET s, int level, int optname,
 *                const char FAR* optval, int optlen);
 * level : SOL_SOCKET, IPPROTO_IP, IPPROTO_IPV6, IPPROTO_TCP, IPPROTO_UDP
 * optname: IPPROTO_TCP->TCP_NODELAY,TCP_BSDURGENT,
 * return: no error 0 or SOCKET_ERROR
 */
int net_sock_cork(int fd, int state)
{
#if WIN32
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&state, sizeof(state));
#else
    return setsockopt(fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
#endif // WIN32
}

int net_send8(int fd, uint8_t n)
{
    uint8_t nbo = n; //htons(n);
    return send(fd, (const char*)&nbo, sizeof(nbo), 0);
}

int net_send16(int fd, uint16_t n)
{
    uint16_t nbo = n; //htons(n);
    return send(fd, (const char*)&nbo, sizeof(nbo), 0);
}

int net_send32(int fd, uint32_t n)
{
    uint32_t nbo = n;  //htonl(n);
    return send(fd, (const char*)&nbo, sizeof(nbo), 0);
    return -1;
}
