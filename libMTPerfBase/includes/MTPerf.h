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
 *   Inspiration Source: iperf
 */

#ifndef MTPERF_H_INCLUDED
#define MTPERF_H_INCLUDED


#include "MTTimer.h"
#include "MTSockUtil.h"

/* default settings */
#define P_TCP SOCK_STREAM
#define P_UDP SOCK_DGRAM
#define DEFAULT_UDP_BLKSIZE 1460 /* default is dynamically set, else this */
#define DEFAULT_TCP_BLKSIZE (128 * 1024)  /* default read/write block size */

/* default settings */
#define DEFAULT_PORT 8989  /* default port to listen on*/
#define DEFAULT_UDP_RATE (1024 * 1024) /* 1 Mbps */
#define DEFAULT_OMIT 0 /* seconds */
#define DEFAULT_DURATION 10 /* seconds */
#define DEFAULT_SOCK_TIMEOUT 1000 /* ms-millisecond */

#define uS_TO_NS 1000
#define SEC_TO_US 1000000LL
#define SEC_TO_NS 1000000000LL	/* too big for enum/const on some platforms */

#define MAX_RESULT_STRING 4096
#define MAX_UDP_BUFFER_EXTRA 1024
#define MAX_BUFFER_SIZE 1024
#define MAX_SLEEP_TIME  100 /*us*/

/* constants for command line arg sanity checks */
#define MB (1024 * 1024)
#define MAX_TCP_BUFFER (512 * MB)
#define MAX_BLOCKSIZE MB
/* Minimum size UDP send is the size of two 32-bit ints followed by a 64-bit int */
#define MIN_UDP_BLOCKSIZE (4 + 4 + 8)
/* Maximum size UDP send is (64K - 1) - IP and UDP header sizes */
#define MAX_UDP_BLOCKSIZE (65535 - 8 - 20)
#define MIN_INTERVAL 0.1
#define MAX_INTERVAL 60.0
#define MAX_TIME 86400
#define MAX_BURST 1000
#define MAX_MSS (9 * 1024)
#define MAX_STREAMS 128
#define CIPHER "123456789"

struct task_stats
{
    int sock;
    /*other parameters*/
    int       mss;                  /* for TCP MSS */
    int       ttl;                  /* IP TTL option */
    int       tos;                  /* type of service bit */
    int       flowlabel;            /* IPv6 flow label */
    char      unit_format;          /* -f */

    /*quality of transfer service*/
    uint64_t  bytes_recv_interval;
    uint64_t  bytes_recv;
    uint32_t  blocks_recv;
    uint64_t  bytes_send_interval;
    uint64_t  bytes_send;           /* number of bytes to send */
    uint32_t  blocks_send;          /* number of blocks (packets) to send */
    uint64_t  rate;                 /* target data rate for application pacing*/
    uint64_t  fqrate;               /* target data rate for FQ pacing*/
    int	      pacing_timer;	        /* pacing timer in microseconds */
    int       burst;                /* packets per burst */

    /*quality of packet*/
    uint32_t packet_index;
    uint32_t packet_cnt_outorder;
    uint32_t packet_cnt_error;

    /*
     * for udp measurements - This can be a structure outside stream, and
     * stream can have a pointer to this
     */
    int       packet_count;
    int	      peer_packet_count;
    int       omitted_packet_count;
    uint64_t  jitter;       /*us*/
    uint64_t  prev_transit; /*us*/
    int       outoforder_packets;
    int       omitted_outoforder_packets;
    int       cnt_error;
    int       omitted_cnt_error;
};

struct addrinfo;
struct task_settings
{
    char      role;                 /* 'c' lient or 's' erver */
    int       rand_cnt;             /* rand test count */

    /* client */
    char  *local_host;
    int    local_port;
    char  *local_address;           /* first -B option */
    struct addrinfo *local_ai;

    /* server*/
    char  *remote_host;             /* -c option */
    int    remote_port;

    /*socket parameters*/
    int       sock_domain;          /*adress family: AF_INET AF_INET6 AF_LOCAL AF_ROUTE*/
    int       sock_type;            /*AF_SOCK_STREAM SOCK_DGRAM SOCK_RAW SOCK_PACKET SOCK_SEQPACKET*/
    int       sock_proto;           /*IPPROTO_TCP IPPTOTO_UDP IPPROTO_SCTP IPPROTO_TIPC*/
    int       sock_bufsize;         /* window size for TCP */
    int	      sock_timeout;	        /* socket connection timeout, in ms */
    int       block_size;           /* size of read/writes(-l)*/
    int       max_fd;
    fd_set    read_set;             /* set of read sockets */
    fd_set    write_set;            /* set of write sockets */

    int       ctrl_sck;
    int       listener;
    int       prot_listener;

     /* proto variables for Options */
    int       omit;                             /* duration of omit period (-O flag) */
    int       duration;                         /* total duration of test (-t flag) */
    char     *diskfile_name;                    /* -F option */
    char     *title;                            /* -T option */
    char     *congestion;                       /* -C option */
    char     *congestion_used;                  /* what was actually used */
    char     *remote_congestion_used;           /* what the other side used */
    char     *logfile;                          /* --logfile option */
    FILE     *outfile;

    /* boolean variables for Options */
    int       no_delay;                         /* -N option */
    int       reverse;                          /* -R option reverse send/recv operations*/
    int	      verbose;                          /* -V option - verbose mode */
    int	      zerocopy;                         /* -Z option - use sendfile */
    int       debug;				            /* -d option - enable debug */
    int	      get_server_output;		        /* --get-server-output */
    int	      udp_counters_64bit;		        /* --use-64-bit-udp-counters */
    int       forceflush;                       /* --forceflush - flushing output at every interval */
    int	      multisend;

    /* Interval related members */
    signed char state;
    int       done;
    int       omitting;
    double    stats_interval;
    double    reporter_interval;
    void      (*stats_callback) (struct task_setting *);
    void      (*reporter_callback) (struct task_setting *);
    Timer     *omit_timer;
    Timer     *timer;
    Timer     *stats_timer;
    Timer     *reporter_timer;
};

#endif // MTPERF_H_INCLUDED
