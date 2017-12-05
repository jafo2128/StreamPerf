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

#pragma once

#include <stdint.h>

#ifndef _MT_RTSP_H_
#define _MT_RTSP_H_

/*
 * This struct represents a session established with the RTSP server
 * so the data contained is used for later PLAY commands
 */
struct rtsp_session {
  int socket;
  char *stream;

  /* RTSP protocol stuff */
    unsigned int packetization; /* Packetization mode from SDP data */
    unsigned int cport_from;    /* client_port from */
    unsigned int cport_to;      /* client port to   */
    unsigned int sport_from;    /* server port from */
    unsigned int sport_to;      /* server port to   */
    unsigned long session;      /* session ID       */
    char session_id[16]; /* session ID       */
};

struct rtsp_params {
    int rtsp_cseq;
    int client_port;
    int opt_verbose;
    int opt_stdout;
    char *opt_stream;
    char *opt_name;
    char *stream_host;
    char *dsp;
    unsigned long stream_port;
    char *stream_dump;
};

/* Last Sender Report timestamp (middle 32 bits) */
//uint32_t rtcp_last_sr_ts;

#define VERSION           "0.1"
#define PROTOCOL_PREFIX   "rtsp://"
#define RTSP_PORT         554
#define RTSP_CLIENT_PORT  9500
#define RTSP_RESPONSE     "RTSP/1.0 "
#define CMD_OPTIONS       "OPTIONS rtsp://%s:%lu RTSP/1.0\r\nCSeq: %i\r\n\r\n"
#define CMD_DESCRIBE      "DESCRIBE %s RTSP/1.0\r\nCSeq: %i\r\nAccept: application/sdp\r\n\r\n"

//#define CMD_SETUP         "SETUP %s/trackID=1 RTSP/1.0\r\nCSeq: %i\r\nTransport: RTP/AVP/TCP;interleaved=0-1;\r\n\r\n"
#define CMD_SETUP         "SETUP %s RTSP/1.0\r\nCSeq: %i\r\nTransport: RTP/AVP/TCP;interleaved=0-1;\r\n\r\n"

#define CMD_PLAY          "PLAY %s RTSP/1.0\r\nCSeq: %i\r\nSession: %s\r\nRange: npt=0.00-\r\n\r\n"

/* Transport header constants */
#define SETUP_SESSION      "Session: "
#define SETUP_TRNS_CLIENT  "client_port="
#define SETUP_TRNS_SERVER  "server_port="

class mt_rtp;
class mt_rtcp;
class mt_streamer;
class mt_rtsp {
public:
    mt_rtsp(struct rtsp_params* params);
    ~mt_rtsp();
    int rtsp_loop();
private:
    int rtsp_connect(char *stream);
    int rtsp_cmd_play(int sock, char *stream, const char* session_id);
    int rtsp_cmd_setup(int sock, char *stream, struct rtsp_session *session);
    int rtsp_cmd_describe(int sock, char *stream, char **sprop);
    int rtsp_cmd_options(int sock, char *stream);
    int rtsp_response_status(char *response, char **error);
    void rtsp_rtcp_reports(int fd);
    void rtsp_header(int fd, int channel, uint16_t length);
    void rtsp_cseq_inc();

private:
    struct rtsp_params* _rtsp_params;
    mt_streamer* _streamer;
    mt_rtcp* _rtcp_client;
    mt_rtp* _rtp_client;
};



#endif /*_MT_RTSP_H_*/
