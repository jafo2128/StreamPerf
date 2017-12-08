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
 *   Original Author: shareviews@sina.com (2017-12-XX)
 */


#include "MTPerfTaskUDP.h"

#include "Define.h"
#include "MTPerf.h"
#include "MTSockUtil.h"

#define TAG "MTPerfTaskUDP"

int  MTPerfTaskUDP::netInit() {
    return 0;
}

/*
 * netListen
 *
 * Start up a listener for UDP stream connections.  Unlike for TCP,
 * there is no listen(2) for UDP.  This socket will however accept
 * a UDP datagram from a client (indicating the client's presence).
 */
int  MTPerfTaskUDP::netListen() {
    int sock;
    if ((sock = mt_sock_bind(mTaskSettings->sa_family, P_UDP,
                            mTaskSettings->local_host, mTaskSettings->remote_port)) < 0) {
        return NET_ERROR_HARD;
    }

    return sock;
}

/*
 * MTPerfTaskUDP::netAccept()
 *
 * Accepts a new UDP "connection"
 */
int  MTPerfTaskUDP::netAccept() {
    struct sockaddr_storage sa_peer;
    int       buf;
    socklen_t len;
    int  err, sock;
    struct task_settings*  setting = mTaskSettings;

    /*
     * Get the current outstanding socket.  This socket will be used to handle
     * data transfers and a new "listening" socket will be created.
     */
    err  = NET_ERROR_NONE;
    sock = setting->prot_listener;

    /*
     * Grab the UDP packet sent by the client.  From that we can extract the
     * client's address, and then use that information to bind the remote side
     * of the socket to the client.
     */
    len = sizeof(sa_peer);
    if ((err = recvfrom(sock, (char*)&buf, sizeof(buf), 0, (struct sockaddr *)&sa_peer, &len)) < NET_ERROR_NONE) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to recvfrom, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    }

    if (connect(sock, (struct sockaddr *)&sa_peer, len) < NET_ERROR_NONE) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to connect, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    }

    /* check and set socket buffer sizes */
    if(mt_sock_check_bufsize(sock, setting->sock_bufsize, setting->block_size, setting->debug) < NET_ERROR_NONE) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to check_bufsize, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    }
    /* increase socket buffer sizes if default is small*/
    if(setting->sock_bufsize == 0) {
        setting->sock_bufsize = setting->block_size + MAX_UDP_BUFFER_EXTRA;
        if(mt_sock_check_bufsize(sock, setting->sock_bufsize, setting->block_size, setting->debug) < NET_ERROR_NONE) {
            MTLog::LogEx(TAG, __FUNCTION__, "Fail to check_bufsize, error:%s", mt_last_sock_error());
            return NET_ERROR_HARD;
        }
    }


    /*
     * Create a new "listening" socket to replace the one we were using before.
     */
    setting->prot_listener = mt_sock_bind(setting->sa_family, P_UDP, setting->local_host, setting->remote_port);
    if (setting->prot_listener < 0) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to mt_sock_bind, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    }

    FD_SET(setting->prot_listener, &setting->read_set);
    setting->max_fd = (setting->max_fd < setting->prot_listener) ? setting->prot_listener : setting->max_fd;

    /* Let the client know we're ready "accept" another UDP "stream" */
    buf = 987654321;		/* any content will work here */
    if (mt_sock_send(sock, (const char*)&buf, sizeof(buf)) < NET_ERROR_NONE) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to mt_sock_send, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    }

    return sock;
}

#define CONNECT_FORMAT "local_host:%-16s local_port:%d use sa_family:%d\n remote_host:%-16s remote_port:%d"
int  MTPerfTaskUDP::netConnect() {
    int ts_buf, sock = 0;
    struct task_settings*  setting = mTaskSettings;

    /* create and bind local sock, and connect server*/
    if(setting->debug) {
        MTLog::LogEx(TAG, __FUNCTION__, CONNECT_FORMAT,
                           setting->local_host,  setting->local_port, setting->sa_family,
                           setting->local_host,  setting->local_port);
    }
    if((sock = mt_sock_connect_server(setting->sa_family, P_UDP,
                     setting->local_host,  setting->local_port,
                     setting->remote_host, setting->remote_port, -1/*timeout*/)) < NET_ERROR_NONE) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to connect_server, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    }

    /* check and set socket buffer sizes */
    if(mt_sock_check_bufsize(sock, setting->sock_bufsize, setting->block_size, setting->debug) < NET_ERROR_NONE) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to check_bufsize, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    }
    /* increase socket buffer sizes if default is small*/
    if(setting->sock_bufsize == 0) {
        setting->sock_bufsize = setting->block_size + MAX_UDP_BUFFER_EXTRA;
        if(mt_sock_check_bufsize(sock, setting->sock_bufsize, setting->block_size, setting->debug) < NET_ERROR_NONE) {
            MTLog::LogEx(TAG, __FUNCTION__, "Fail to check_bufsize, error:%s", mt_last_sock_error());
            return NET_ERROR_HARD;
        }
    }

#ifdef SO_RCVTIMEO
    /* enable timeout for socket. */
    struct timeval tv;
    tv.tv_sec  = 3;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));
#endif

    /*
     * Let the server know we're here.
     * The server learns our address by obtaining its peer's address.
     */
    ts_buf = 123456789;		/* this can be pretty much anything */
    if(mt_sock_send(sock, (const char*)&ts_buf, sizeof(ts_buf)) < NET_ERROR_NONE) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to mt_sock_send, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    }

    /*
     * Wait until the server replies back to us.
     */
    if(mt_sock_recv(sock, (char*)&ts_buf, sizeof(ts_buf)) < NET_ERROR_NONE) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to mt_sock_recv, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    }

    return NET_ERROR_NONE;
}
int  MTPerfTaskUDP::netSend() {
    int err = NET_ERROR_NONE;
    uint32_t  sec, usec, pack_cnt;
    struct timeval now_tv;
    struct task_settings*  setting = mTaskSettings;
    struct task_stats*     stats   = mTaskStats;

    mTaskStats->packet_index++;
    gettimeofday(&now_tv, NULL);
    sec      = htonl(now_tv.tv_sec);
    usec     = htonl(now_tv.tv_usec);
    pack_cnt = htonl(mTaskStats->packet_index);
    memcpy(mTsBuffer,   &sec,      sizeof(sec));
    memcpy(mTsBuffer+4, &usec,     sizeof(usec));
    memcpy(mTsBuffer+8, &pack_cnt, sizeof(pack_cnt));
    err = mt_sock_send(stats->sock, mTsBuffer, setting->block_size);
    if (err < NET_ERROR_NONE){
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to mt_sock_send, error:%s", mt_last_sock_error());
        mTaskStats->packet_index--;
        return NET_ERROR_HARD;
    }else{
        mTaskStats->bytes_send += err;
        mTaskStats->blocks_recv++;
        mTaskStats->bytes_send_interval += err;
    }

    if(setting->debug) {
        MTLog::LogEx(TAG, __FUNCTION__, "sent %d bytes, total %lld packets, content:%12.12s",
                          setting->block_size, mTaskStats->packet_index, mTsBuffer);
    }

    return err;
}

int  MTPerfTaskUDP::netRecv() {
    int err = NET_ERROR_NONE;
    uint32_t pack_index;
    struct timeval send_tv, recv_tv;
    struct task_settings*  setting = mTaskSettings;

    uint64_t transit = 0, transit_diff = 0;
    err = mt_sock_recv(mTaskStats->sock, mTsBuffer, setting->block_size);

    /*
     * If we got an error in the read, or if we didn't read anything
     * because the underlying read(2) got a EAGAIN, then skip packet
     * processing.
     */
    if (err < NET_ERROR_NONE) {
        MTLog::LogEx(TAG, __FUNCTION__, "Fail to mt_sock_send, error:%s", mt_last_sock_error());
        return NET_ERROR_HARD;
    } else {
        mTaskStats->blocks_recv++;
        mTaskStats->bytes_recv += err;
        mTaskStats->bytes_recv_interval += err;
    }

	memcpy(&(send_tv.tv_sec),  mTsBuffer,   sizeof(send_tv.tv_sec));
	memcpy(&(send_tv.tv_usec), mTsBuffer+4, sizeof(send_tv.tv_usec));
	memcpy(&pack_index, mTsBuffer+8, sizeof(pack_index));
	send_tv.tv_sec  = ntohl(send_tv.tv_sec);
	send_tv.tv_usec = ntohl(send_tv.tv_usec);
	pack_index = ntohl(pack_index);

    if (setting->debug){
        MTLog::LogEx(TAG, __FUNCTION__, "pack_index = %d; packet_index=%d",
                                         pack_index, mTaskStats->packet_index);
    }

    /*
     * Try to handle out of order packets.
     * It's not rigorous and accurate
     */
    if (pack_index >= mTaskStats->packet_index + 1) {

	     /* Forward, but is there a gap in sequence numbers? */
        if (pack_index > mTaskStats->packet_index + 1) {
	    /* There's a gap so count that as a loss. */
            mTaskStats->packet_cnt_error += (pack_index - 1) - mTaskStats->packet_index;
        }
	    /* Update the highest sequence number seen so far. */
        mTaskStats->packet_index = pack_index;
    } else {
        /*Sequence number went backward--out-of-order packet.*/
        mTaskStats->packet_cnt_outorder++;

        /*
         * If we have lost packets, then the fact that we are now
         * seeing an out-of-order packet offsets a prior sequence
         * number gap that was counted as a loss.  So we can take
         * away a loss.
         */
        if (mTaskStats->packet_cnt_error > 0) mTaskStats->packet_cnt_error--;

        /* Log the out-of-order packet */
        if (setting->debug){
            MTLog::LogEx(TAG, __FUNCTION__, "OUT OF ORDER; incoming_sequence =%d, but expected incoming_sequence=%d",
                                             pack_index, mTaskStats->packet_index);
        }
    }

    /*
     * jitter measurement
     *
     * This computation is based on RFC 1889 (specifically
     * sections 6.3.1 and A.8).
     *
     * Note that synchronized clocks are not required since
     * the source packet delta times are known.  Also this
     * computation does not require knowing the round-trip
     * time.
     */
    gettimeofday(&recv_tv, NULL);
    transit = timeval_diff(&send_tv, &recv_tv);
    transit_diff = transit - mTaskStats->prev_transit;
    transit_diff = (transit_diff>0)?transit_diff:(-transit_diff);
    mTaskStats->prev_transit = transit;
    mTaskStats->jitter += (transit_diff - mTaskStats->jitter) / 16.0;

    return err;
}

/**
 ** Class MTPerfTaskUDP(Server)
 **/
int MTPerfTaskUDP::doTask(void* args) {
    return NET_ERROR_HARD;
}

/**
 ** Class MTPerfTaskUDP(Client)
 **/
int MTPerfTaskUDP::doTask2(void* args) {
    return NET_ERROR_HARD;
}
