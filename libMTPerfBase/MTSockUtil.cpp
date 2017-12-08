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

#include "MTSockUtil.h"
#include "MTLog.h"
#include <unistd.h>

#define TAG "MTSockUtil"

const char* mt_last_sock_error(){
    return strerror(errno);
}

/*
 * Set socket buffer size if requested.  Do this for both sending and
 * receiving so that we can cover both normal and --reverse operation.
 */
int mt_sock_check_bufsize(int sock, int sock_bufsize, int block_size, int debug){
    int opt;
    int sndbuf_actual, rcvbuf_actual;
    socklen_t optlen;

    if ((opt = sock_bufsize) > 0) {
        if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char*)&opt, sizeof(opt)) < 0) {
            MTLog::LogEx(TAG, __FUNCTION__, "Fail to setsockopt, error:%s", mt_last_sock_error());
            return NET_ERROR_HARD;
        }
        if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char*)&opt, sizeof(opt)) < 0) {
            MTLog::LogEx(TAG, __FUNCTION__, "Fail to setsockopt, error:%s", mt_last_sock_error());
            return NET_ERROR_HARD;
        }
    }

    /* Read back and verify the sender socket buffer size */
    optlen = sizeof(sndbuf_actual);
    if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&sndbuf_actual, &optlen) < 0) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to getsockopt, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    }

    if ((sock_bufsize > 0) && (sock_bufsize > sndbuf_actual)) {
        MTLog::LogEx(TAG, __FUNCTION__, "SO_SNDBUF is %u, expecting %u", sndbuf_actual, sock_bufsize);
        return NET_ERROR_HARD;
    }

    if (debug || (block_size > sndbuf_actual)) {
       MTLog::LogEx(TAG, __FUNCTION__, "block_size(%u);sock_bufsize(%u);sndbuf_actual(%u)",
                    block_size, sock_bufsize, sndbuf_actual);
    }

    /* Read back and verify the receiver socket buffer size */
    optlen = sizeof(rcvbuf_actual);
    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&rcvbuf_actual, &optlen) < 0) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to getsockopt, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    }
    if ((sock_bufsize > 0) && (sock_bufsize > rcvbuf_actual)) {
        MTLog::LogEx(TAG, __FUNCTION__, "SO_RCVBUF is %u, expecting %u", rcvbuf_actual, sock_bufsize);
        return NET_ERROR_HARD;
    }
    if (debug || (block_size > rcvbuf_actual)) {
       MTLog::LogEx(TAG, __FUNCTION__, "block_size(%u);sock_bufsize(%u);rcvbuf_actual(%u)",
                                        block_size,  sock_bufsize, rcvbuf_actual);
    }

    return NET_ERROR_NONE;
}

int mt_sock_connect(int sock, const struct sockaddr *sock_addr, socklen_t sock_addr_len, int timeout) {
	int err, last_err, flags;
    err = last_err = NET_ERROR_NONE;
	if (timeout != -1) {
    #ifdef WIN32
        flags=1;
        ioctlsocket(sock, FIONBIO, (unsigned long *)&flags); //set NONBLOCK
    #else
        flags = fcntl(sock, F_GETFL, 0);
        if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1){
            last_err = errno;
            err      = NET_ERROR_HARD;
        }
    #endif // WIN32
	}


	if ((err = connect(sock, sock_addr, sock_addr_len)) != 0 ) {
        err      = NET_ERROR_HARD;
        last_err = errno;
	}
	if(timeout != -1) {
    #ifdef WIN32
        flags = 0;
        ioctlsocket(sock, FIONBIO, (unsigned long *)&flags); //clear NONBLOCK
    #else
        if (fcntl(sock, F_SETFL, flags) == -1) {
            last_err = errno;
            err      = NET_ERROR_HARD;
        }
    #endif // WIN32
    }
    errno = last_err;
	return err;
}

int mt_sock_connect_server(int sa_family, int proto,
                          char *local_host,  int local_port,
                          char *remote_host, int remote_port, int timeout) {
    struct addrinfo hints;
    struct addrinfo* local_result  = NULL;
    struct addrinfo* remote_result = NULL;
    struct addrinfo* result;
    int sock, err, last_err;

    sock = err = last_err = NET_ERROR_NONE;
    if (local_host) {
        local_result = NULL;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family   = sa_family;
        hints.ai_socktype = proto;
        if (getaddrinfo(local_host, NULL, &hints, &local_result) != 0) {
            local_result = NULL;
            err          = NET_ERROR_HARD;
            last_err     = errno;
            goto TAG_ERROR;
        }
    }

    if(remote_host) {
        memset(&hints, 0, sizeof(hints));
        hints.ai_family   = sa_family;
        hints.ai_socktype = proto;
        if (getaddrinfo(remote_host, NULL, &hints, &remote_result) != 0) {
            remote_result = NULL;
            err           = NET_ERROR_HARD;
            last_err      = errno;
            goto TAG_ERROR;
        }
    }

    sock = socket(remote_result->ai_family, proto, 0);
    if ((sock = socket(remote_result->ai_family, proto, 0)) < 0) {
        sock     = 0;
        err      = NET_ERROR_HARD;
        last_err = errno;
	    goto TAG_ERROR;
    }

    result = local_result;
    if (NULL != result) {
        if (local_port) {
            struct sockaddr_in *local_addr = (struct sockaddr_in *)result->ai_addr;
            local_addr->sin_port = htons(local_port);
            result->ai_addr  = (struct sockaddr *)local_addr;
        }

        if (bind(sock, (struct sockaddr *) result->ai_addr, result->ai_addrlen) < 0) {
            err      = NET_ERROR_HARD;
            last_err = errno;
            goto TAG_ERROR;
        }
    }

    result = remote_result;
    if (NULL != result) {
        if (remote_port) {
            struct sockaddr_in *remote_addr = (struct sockaddr_in *)result->ai_addr;
            remote_addr->sin_port = htons(remote_port);
            result->ai_addr  = (struct sockaddr *)remote_addr;
        }

        if (mt_sock_connect(sock, (struct sockaddr *) result->ai_addr, result->ai_addrlen, timeout) < 0) {
            err      = NET_ERROR_HARD;
            last_err = errno;
            goto TAG_ERROR;
        }
    }
TAG_ERROR:
    if(NULL != local_result) {
        freeaddrinfo(local_result);
        local_result = NULL;
    }
    if(NULL != remote_result) {
        freeaddrinfo(remote_result);
        remote_result = NULL;
    }
    if(NET_ERROR_NONE != err) {
        if(sock > 0) close(sock);
        errno = last_err;
        return err;
    }
    return sock;
}

int mt_sock_bind(int sa_family, int proto, char *host_name, int host_port) {
    int sock, opt, err, last_err;
    struct addrinfo  hints;
    struct addrinfo* result = NULL;

    sock = err = last_err = NET_ERROR_NONE;
    memset(&hints, 0, sizeof(struct addrinfo));
    if (sa_family == AF_UNSPEC && !host_name) {
        hints.ai_family = AF_INET6;
    } else {
        hints.ai_family = sa_family;
    }
    hints.ai_socktype = proto;
    hints.ai_flags    = AI_PASSIVE;
    if (getaddrinfo(host_name, NULL,/*port*/ &hints, &result) != 0) {
        result = NULL;
        err = NET_ERROR_HARD;
        last_err = errno;
        goto TAG_ERROR;
     } else {
        struct sockaddr_in *sock_addr = (struct sockaddr_in *)result->ai_addr;
        sock_addr->sin_port = htons(host_port);
        result->ai_addr  = (struct sockaddr *)sock_addr;
     }

    if((sock = socket(result->ai_family, proto, 0)) < 0) {
        err = NET_ERROR_HARD;
        last_err = errno;
        goto TAG_ERROR;
    }

    opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
        err = NET_ERROR_HARD;
        last_err = errno;
        goto TAG_ERROR;
    }

    if (bind(sock, (struct sockaddr *) result->ai_addr, result->ai_addrlen) < 0) {
        err = NET_ERROR_HARD;
        last_err = errno;
        goto TAG_ERROR;
    }

    if ((proto == SOCK_STREAM)&&(listen(sock, 5) < 0)) {
        err = NET_ERROR_HARD;
        last_err = errno;
        goto TAG_ERROR;
    }

TAG_ERROR:
    if(NULL != result) {
        freeaddrinfo(result);
        result = NULL;
    }
    if(NET_ERROR_NONE != err) {
        if(sock > 0) close(sock);
        errno = last_err;
        return err;
    }

    return sock;
}

int mt_sock_send(int sock, const char *buffer, size_t count) {
    int err  = NET_ERROR_NONE;
    size_t nleft = count;

    while (nleft > 0) {
        err = write(sock, buffer, nleft);
        if (err < 0) {
            switch (errno) {
            case EINTR:
            case EAGAIN:
                return count - nleft;
            default:
                return NET_ERROR_HARD;
            }
        } else if (err == 0) {
            return NET_ERROR_SOFT;
        } else {
            nleft  -= err;
            buffer += err;
        }
    }
    return count;
}

int mt_sock_recv(int sock, char *buffer, size_t count) {
    int err  = NET_ERROR_NONE;
    size_t nleft = count;

    while (nleft > 0) {
        err = read(sock, buffer, nleft);
        if (err < 0) {
            if (errno == EINTR || errno == EAGAIN)
                break;
            else
                return NET_ERROR_HARD;
        } else if (err == 0){
            return NET_ERROR_SOFT;
        } else {
            nleft  -= err;
            buffer += err;
        }
    }
    return count - nleft;
}

/*
 * http://blog.csdn.net/hnlyyk/article/details/50856268
 */
int mt_sock_sendfile(int from_fd, int to_sock, size_t count) {
#ifdef WIN32
    return NET_ERROR_NONE;
#else
    off_t   offset;
    ssize_t err   = NET_ERROR_NONE;
    size_t  nleft = count;
    while (nleft > 0) {
	    offset = count - nleft;
        err = sendfile(to_sock, from_fd, &offset, nleft);
        if (err < 0) {
            switch (errno) {
                case EINTR:
                case EAGAIN:
        #if (EAGAIN != EWOULDBLOCK)
                case EWOULDBLOCK:
        #endif
                if (count == nleft)
                    return NET_ERROR_SOFT;
                return count - nleft;

                case ENOBUFS:
                case ENOMEM:
                return NET_ERROR_SOFT;

                default:
                return NET_ERROR_HARD;
            }
        } else if (err == 0) {
            return NET_ERROR_SOFT;
        } else {
            nleft  -= err;
        }
    }
    return count;
#endif // WIN32
}

int mt_sock_set_nonblocking(int sock, int nonblocking) {
    int err = NET_ERROR_NONE;
#ifdef WIN32

#else
    int flags, newflags;
    flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0) {
        perror("Fail to fcntl(sock=%d, F_GETFL)", sock);
        return -1;
    }
    if (nonblocking)
        newflags = flags | (int) O_NONBLOCK;
    else
        newflags = flags & ~((int) O_NONBLOCK);

	if ((newflags != flags) && (fcntl(fd, F_SETFL, newflags) < 0)) {
	    perror("Fail to fcntl(sock=%d, F_SETFL)", sock);
	    return -1;
	}
#endif
    return err;
}

int mt_sock_get_sa_family(int sock) {
    struct sockaddr_storage sa;
    socklen_t len = sizeof(sa);

    if (getsockname(sock, (struct sockaddr *)&sa, &len) < 0) {
        return -1;
    }
    return ((struct sockaddr *)&sa)->sa_family;
}
