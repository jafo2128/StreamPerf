/*
 *      Copyright (C) 2017-2020 MediaTime
 *      http://media-tm.com (shareviews@sina.com)
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with MediaTime; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 *      Original Author: shareviews@sina.com (2017-12-XX)
 */

#ifndef MT_SOCK_UTIL_H_INCLUDED
#define MT_SOCK_UTIL_H_INCLUDED


#define _WIN32_WINNT_WINXP                  0x0501 // Windows XP
#define _WIN32_WINNT_WIN7                   0x0601 // Windows 7
#define _WIN32_WINNT_WIN8                   0x0602 // Windows 8
#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10

#include <errno.h>
#ifdef WIN32
    #define _WIN32_WINNT _WIN32_WINNT_WIN7
    #include <ws2tcpip.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/sendfile.h>
    #define closesocket close
#endif

typedef enum {
    PERF_ERROR_NONE   = 0,
    PERF_ERROR_SOCK   = 1,
    PERF_ERROR_BIND   = 2,
    PERF_ERROR_ADDR   = 3,
    PERF_ERROR_TRANS  = 4,
} PERF_TASK_ERROR;

typedef enum {
    NET_ERROR_SOFT   = -2,
    NET_ERROR_HARD   = -1,
    NET_ERROR_NONE   = 0,
} MT_sock_ERROR_TYPE;

const char* mt_last_sock_error();
int mt_sock_check_bufsize(int sock, int sock_bufsize, int block_size, int debug=0);
int mt_sock_connect(int sock, const struct sockaddr *sock_addr, socklen_t sock_addr_len, int timeout);
int mt_sock_connect_server(int sa_family, int proto,
                          char *local_host,  int local_port,
                          char *remote_host, int remote_port, int timeout);
int mt_sock_bind(int sa_family, int proto, char *host_name, int host_port);
int mt_sock_send(int sock, const char *buf, size_t count);
int mt_sock_recv(int sock, char *buf, size_t count);
int mt_sock_sendfile(int from_fd, int to_sock, size_t count);
int mt_sock_set_nonblocking(int sock, int nonblocking);

/*
 *  sa_family--socket address family (AF_INET,AF_INET6/AF_UNSPEC)
 */
int mt_sock_get_sa_family(int sock);

#endif // MTNETUTIL_H_INCLUDED
