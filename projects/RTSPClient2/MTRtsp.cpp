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

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

/* local headers */
#include "MTRtsp.h"
#include "MTNetwork.h"
#include "MTStreamer.h"
#include "MTBase64.h"
#include "MTRtcp.h"
#include "MTRtp.h"

static int opt_verbose = 0;

mt_rtsp::mt_rtsp(struct rtsp_params* params) {
    opt_verbose  =  1; //_rtsp_params->opt_verbose;
    _rtsp_params = params;
    _streamer    = new mt_streamer();
    _rtp_client  = new mt_rtp();
    _rtcp_client = new mt_rtcp(_rtp_client);
    _streamer->get_params()->stream_dump = params->stream_dump;
}

mt_rtsp::~mt_rtsp() {

}

void mt_rtsp::rtsp_cseq_inc()
{
    _rtsp_params->rtsp_cseq++;
}

void mt_rtsp::rtsp_header(int fd, int channel, uint16_t length)
{
    uint8_t tmp_8;
    uint16_t tmp_16;

    tmp_8 = 0x24;      /* RTSP magic number */
    net_send8(fd, tmp_8);

    tmp_8 = channel;   /* channel */
    net_send8(fd, tmp_8);

    tmp_16 = length;
    net_send16(fd, tmp_16);
}

void mt_rtsp::rtsp_rtcp_reports(int fd)
{
    //    net_sock_cork(fd, 1);
    rtsp_header(fd, 1, 74);

    /* report 1 */
    _rtcp_client->rtcp_receiver_report(fd);       /* 32 bytes */
    _rtcp_client->rtcp_receiver_desc(fd);         /* 17 bytes */

    usleep(200);
    /* report 0 */
    //rtcp_receiver_report_zero(fd);  /*  8 bytes */
    //rtcp_receiver_desc(fd);         /* 17 bytes */

    //net_sock_cork(fd, 0);

    printf("================RTCP SENT!=================\n");
}

/*
 * Returns the RTSP status code from the response, if an error occurred it
 * allocates a memory buffer and store the error message on 'error' variable
 */
int mt_rtsp::rtsp_response_status(char *response, char **error)
{
    int size = 256;
    int err_size;
    int offset = sizeof(RTSP_RESPONSE) - 1;
    char buf[8];
    char *sep;
    char *eol;
    *error = NULL;

    if (strncmp(response, RTSP_RESPONSE, offset) != 0) {
        *error = (char*)malloc(size);
        snprintf(*error, size, "Invalid RTSP response format-1");
        return -1;
    }

    sep = strchr(response + offset, ' ');
    if (!sep) {
        *error = (char*)malloc(size);
        snprintf(*error, size, "Invalid RTSP response format-2");
        return -1;
    }

    memset(buf, '\0', sizeof(buf));
    strncpy(buf, response + offset, sep - response - offset);

    eol = strchr(response, '\r');
    err_size = (eol - response) - offset - 1 - strlen(buf);
    *error = (char*)malloc(err_size + 1);
    strncpy(*error, response + offset + 1 + strlen(buf), err_size);

    return atoi(buf);
}

int mt_rtsp::rtsp_cmd_options(int sock, char *stream)
{
    int n;
    int ret = 0;
    int status;
    int size = 4096;
    char *err;
    char buf[size];

    RTSP_INFO("OPTIONS: command\n");

    memset(buf, '\0', sizeof(buf));
    n = snprintf(buf, size, CMD_OPTIONS, _rtsp_params->stream_host, _rtsp_params->stream_port, _rtsp_params->rtsp_cseq);
    DEBUG_REQ(buf);
    n = send(sock, buf, n, 0);

    RTSP_INFO("OPTIONS: command  [send content]\n%s", buf);

    memset(buf, '\0', sizeof(buf));
    n = recv(sock, buf, size - 1, 0);
    if (n <= 0) {
        printf("Error: Server did not respond properly, closing...");
        close(sock);
        exit(EXIT_FAILURE);
    }

    status = rtsp_response_status(buf, &err);
    if (status == 200) {
        RTSP_INFO("OPTIONS: response status %i (%i bytes)\n", status, n);
        RTSP_INFO("OPTIONS: command  [recv content]\n%s", buf);
    }
    else {
        RTSP_INFO("OPTIONS: response status %i: %s\n", status, err);
        ret = -1;
    }

    DEBUG_RES(buf);
    rtsp_cseq_inc();

    return ret;
}

int mt_rtsp::rtsp_cmd_describe(int sock, char *stream, char **sprop)
{
    int n;
    int ret = 0;
    int status;
    int size = 4096;
    char *p, *end;
    char *err;
    char buf[size];

    RTSP_INFO("DESCRIBE: command\n");

    memset(buf, '\0', sizeof(buf));
    n = snprintf(buf, size, CMD_DESCRIBE, stream, _rtsp_params->rtsp_cseq);
    DEBUG_REQ(buf);
    n = send(sock, buf, n, 0);

    RTSP_INFO("DESCRIBE: request sent\n");

    memset(buf, '\0', sizeof(buf));
    n = recv(sock, buf, size - 1, 0);
    if (n <= 0) {
        printf("Error: Server did not respond properly, closing...\n");
        close(sock);
        exit(EXIT_FAILURE);
    }

    status = rtsp_response_status(buf, &err);
    if (status == 200) {
        RTSP_INFO("DESCRIBE: response status %i (%i bytes)\n", status, n);
    }
    else {
        RTSP_INFO("DESCRIBE: response status %i: %s\n", status, err);
        ret = -1;
    }

    DEBUG_RES("%s\n", buf);
    rtsp_cseq_inc();

    /* set the DSP information */
    p = strstr(buf, "\r\n\r\n");
    if (!p) {
        RTSP_INFO("DESCRIBE: invalid DSP information, no rnrn\n");
        return 0;
    }
    DEBUG_RES("DSP information:%s\n", p);

    /* Create buffer for DSP */
    _rtsp_params->dsp = (char*)malloc(n + 1);
    memset(_rtsp_params->dsp, '\0', n + 1);
    strcpy(_rtsp_params->dsp, p + 4);

    /* sprop-parameter-sets key */
    p = strstr(_rtsp_params->dsp, RTP_SPROP);
    if (!p) {
        RTSP_INFO("DESCRIBE: invalid , sprop-parameter-sets key\n");
        return 0;
    }

    end = strchr(p, '\r');
    if (!end) {
        RTSP_INFO("DESCRIBE: invalid end mark, no r\n");
        return -1;
    }

    int prop_size = (end - p) - sizeof(RTP_SPROP) + 1;
    *sprop = (char*)malloc(prop_size + 1);
    memcpy(*sprop, p + sizeof(RTP_SPROP) - 1, prop_size);

    return ret;
}

int mt_rtsp::rtsp_cmd_setup(int sock, char *stream, struct rtsp_session *session)
{
    int n;
    int ret = 0;
    int status;
    int field_size = 16;
    int size = 4096;
    int client_port_from = -1;
    int client_port_to = -1;
    int server_port_from = -1;
    int server_port_to = -1;
    unsigned long session_id;
    char *p;
    char *sep;
    char *err;
    char buf[size];
    char field[field_size];

    RTSP_INFO("SETUP: command\n");

    memset(buf, '\0', sizeof(buf));
    //n = snprintf(buf, size, CMD_SETUP, stream, rtsp_cseq, client_port, client_port + 1);
    n = snprintf(buf, size, CMD_SETUP, stream, _rtsp_params->rtsp_cseq);

    DEBUG_REQ(buf);
    n = send(sock, buf, n, 0);

    RTSP_INFO("SETUP: request sent\n");

    memset(buf, '\0', sizeof(buf));
    n = recv(sock, buf, size - 1, 0);
    if (n <= 0) {
        printf("Error: Server did not respond properly, closing...");
        close(sock);
        exit(EXIT_FAILURE);
    }

    status = rtsp_response_status(buf, &err);
    if (status == 200) {
        RTSP_INFO("SETUP: response status %i (%i bytes)\n", status, n);

        /* Fill session data */
        p = strstr(buf, "Transport: ");
        if (!p) {
            RTSP_INFO("SETUP: Error, Transport header not found\n");
            DEBUG_RES(buf);
            return -1;
        }

        /*
         * Commenting out this code, this part was written to support
         * RTP connection over UDP socket and determinate the server
         * ports to connect.
         *
         * By now the program is using TCP in Intervealed mode, so no
         * extra ports are required.

        buf_client_port = strstr(p, SETUP_TRNS_CLIENT);
        buf_server_port = strstr(p, SETUP_TRNS_SERVER);

        if (!buf_client_port || !buf_server_port) {
            RTSP_INFO("SETUP: Error, ports not defined in Transport header\n");
            DEBUG_RES(buf);
            return -1;
        }

         client_port from
        sep = strchr(buf_client_port + sizeof(SETUP_TRNS_CLIENT) - 1, '-');
        if (!sep) {
            RTSP_INFO("SETUP: client_port have an invalid format\n");
            DEBUG_RES(buf);
            return -1;
        }

        memset(field, '\0', sizeof(field));
        strncpy(field,
                buf_client_port + sizeof(SETUP_TRNS_CLIENT) - 1,
                sep - buf_client_port - sizeof(SETUP_TRNS_CLIENT) + 1);

        client_port_from = atoi(field);

        client_port to
        p = strchr(sep, ';');
        if (!p) {
            p = strchr(sep, '\r');
            if (!p) {
                RTSP_INFO("SETUP: client_port have an invalid format\n");
                DEBUG_RES(buf);
                return -1;
            }
        }

        memset(field, '\0', sizeof(field));
        strncpy(field, sep + 1, p - sep - 1);
        client_port_to = atoi(field);

         server_port from
        sep = strchr(buf_server_port + sizeof(SETUP_TRNS_SERVER) - 1, '-');
        if (!sep) {
            RTSP_INFO("SETUP: server_port have an invalid format\n");
            DEBUG_RES(buf);
            return -1;
        }


        memset(field, '\0', sizeof(field));
        strncpy(field,
                buf_server_port + sizeof(SETUP_TRNS_SERVER) - 1,
                sep - buf_server_port - sizeof(SETUP_TRNS_SERVER) + 1);

        server_port_from = atoi(field);

         server_port to
        p = strchr(sep, ';');
        if (!p) {
            p = strchr(sep, '\r');
            if (!p) {
                RTSP_INFO("SETUP: server_port have an invalid format\n");
                DEBUG_RES(buf);
                return -1;
            }
        }

        memset(field, '\0', sizeof(field));
        strncpy(field, sep + 1, p - sep - 1);
        server_port_to = atoi(field);
        */

        /* Session ID */
        p = strstr(buf, SETUP_SESSION);
        if (!p) {
            RTSP_INFO("SETUP: Session header not found\n");
            DEBUG_RES(buf);
            return -1;
        }

        sep = strchr(p, ';');
        memset(field, '\0', sizeof(field));
        strncpy(field, p + sizeof(SETUP_SESSION) - 1, sep - p - sizeof(SETUP_SESSION) + 1);
        strncpy(session->session_id, field, 16);
        session_id = atol(field);
    }
    else {
        RTSP_INFO("SETUP: response status %i: %s\n", status, err);
        DEBUG_RES(buf);
        return -1;
    }

    /* Fill session data */
    session->packetization = 1; /* FIXME: project specific value */
    session->cport_from = client_port_from;
    session->cport_to   = client_port_to;
    session->sport_from = server_port_from;
    session->sport_to   = server_port_to;
    session->session    = session_id;

    DEBUG_RES(buf);
    rtsp_cseq_inc();
    return ret;
}

int mt_rtsp::rtsp_cmd_play(int sock, char *stream, const char* session_id)
{
    int n;
    int ret = 0;
    int status;
    int size = 4096;
    char *err;
    char buf[size];

    RTSP_INFO("PLAY: command\n");

    memset(buf, '\0', sizeof(buf));
    n = snprintf(buf, size, CMD_PLAY, stream, _rtsp_params->rtsp_cseq, session_id);
    DEBUG_REQ(buf);
    n = send(sock, buf, n, 0);

    RTSP_INFO("PLAY: request sent\n");

    memset(buf, '\0', sizeof(buf));

    n = recv(sock, buf, size - 1, 0);
    if (n <= 0) {
        printf("Error: Server did not respond properly, closing...");
        close(sock);
        exit(EXIT_FAILURE);
    }

    status = rtsp_response_status(buf, &err);
    if (status == 200) {
        RTSP_INFO("PLAY: response status %i (%i bytes)\n", status, n);
    }
    else {
        RTSP_INFO("PLAY: response status %i: %s\n", status, err);
        ret = -1;
    }

    DEBUG_RES(buf);
    rtsp_cseq_inc();
    return ret;
}


int mt_rtsp::rtsp_connect(char *stream)
{
    char *host;
    char *sep;
    char buf[8];
    uint8_t len;
    int pos;
    int offset = sizeof(PROTOCOL_PREFIX) - 1;

    /* Lookup the host address */
    if (!(sep = strchr(stream + offset, ':'))) {
        sep = strchr(stream + offset, '/');
    }

    if (!sep) {
        printf("Error: Invalid stream address '%s'", stream);
        exit(EXIT_FAILURE);
    }
    len = (sep - stream) - offset;
    host = (char*)malloc(len + 1);
    strncpy(host, stream + offset, len);
    host[len] = '\0';

    /* Lookup TCP port if specified */
    sep = strchr(stream + offset + 1, ':');
    if (sep) {
        pos = (sep - stream) + 1;
        sep = strchr(stream + pos, '/');
        if (!sep) {
            printf("Error: Invalid stream address '%s'\n", stream);
            exit(EXIT_FAILURE);
        }

        len = (sep - stream) - pos;
        if (len > sizeof(buf) - 1) {
            printf("Error: Invalid TCP port in stream address '%s'\n", stream);
            exit(EXIT_FAILURE);
        }

        memset(buf, '\0', sizeof(buf));
        strncpy(buf, stream + pos, len);
        _rtsp_params->stream_port = atol(buf);

    }

    _rtsp_params->stream_host = host;
    RTSP_INFO("Connecting to host '%s' port %lu...\n", _rtsp_params->stream_host, _rtsp_params->stream_port);
    return net_tcp_connect(_rtsp_params->stream_host, _rtsp_params->stream_port);
}

int mt_rtsp::rtsp_loop()
{
     int fd;
     int ret;
     struct rtsp_session rtsp_s;
     pid_t streamer_pid;

     /* Connect to server */
     fd = rtsp_connect(_rtsp_params->opt_stream);
     if (fd <= 0) {
         return -1;
     }

     ret = rtsp_cmd_options(fd, _rtsp_params->opt_stream);
     if (ret != 0) {
         return -1;
     }

     char *params;
     ret = rtsp_cmd_describe(fd, _rtsp_params->opt_stream, &params);
     if (ret != 0) {
         printf("Error: Could not send DESCRIBE command to RTSP server\n");
         return -1;
     }

     rtsp_s.socket = fd;
     rtsp_s.stream = strdup(_rtsp_params->opt_stream);

     rtsp_cmd_setup(fd, _rtsp_params->opt_stream, &rtsp_s);
     rtsp_cmd_play(fd,  _rtsp_params->opt_stream, rtsp_s.session_id);

     _rtp_client->rtp_stats_reset();
     _rtp_client->get_stats()->rtp_identifier = rtsp_s.session;
     //rtsp_rtcp_reports(fd);

     /* H264 Parameters, taken from the SDP output */
     int p_size;
     char *sep;
     char *sps;
     char *pps;
     uint8_t *sps_dec;
     uint8_t *pps_dec;
     size_t sps_len;
     size_t pps_len;

     /* SPS */
     sep = strchr(params, ',');
     p_size = (sep - params);
     sps = (char*)malloc(p_size + 1);
     memset(sps, '\0', p_size + 1);
     memcpy(sps, params, p_size);

     /* PPS */
     p_size = (strlen(params) - p_size);
     pps = (char*)malloc(p_size + 1);
     memset(pps, '\0', p_size + 1);
     memcpy(pps, sep + 1, p_size);

     /* Decode each parameter */
     sps_dec = base64_decoder((const uint8_t *) sps, strlen(sps), &sps_len);
     pps_dec = base64_decoder((const uint8_t *) pps, strlen(pps), &pps_len);

     free(sps);
     free(pps);

     int channel;
     int r;
     uint32_t max_buf_size = 1000000;

     uint8_t raw[max_buf_size];
     uint8_t raw_tmp[max_buf_size];
     uint32_t raw_length;
     uint32_t raw_offset = 0;
     uint32_t rtp_length;

     /* open debug file */
     if (_rtsp_params->stream_dump) {
         _streamer->get_params()->stream_fs_fd = open(_rtsp_params->stream_dump, O_CREAT|O_WRONLY|O_TRUNC, 0666);
     }
     else {
         _streamer->get_params()->stream_fs_fd = -1;
     }

     /* write H264 header */
     _streamer->streamer_write_h264_header(sps_dec, sps_len, pps_dec, pps_len);

     /* Create unix named pipe */
     _streamer->get_params()->stream_sock = _streamer->streamer_prepare(_rtsp_params->opt_name, sps_dec, sps_len, pps_dec, pps_len);
     if (_streamer->get_params()->stream_sock <= 0) {
         printf("Error: could not create unix socket\n\n");
         exit(EXIT_FAILURE);
     }

     /* Local pipe */
     _streamer->streamer_pipe_init(_streamer->get_params()->stream_pipe);
     streamer_pid = _streamer->streamer_loop(_streamer->get_params()->stream_sock);

     printf("Streaming will start shortly...\n");

     /* Set recv timeout for fd */
     struct timeval tv;
     tv.tv_sec = 15;  /* 30 Secs Timeout */
     tv.tv_usec = 0;  // Not init'ing this can cause strange errors
     setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

     int send_rtcp = 0;
     _rtcp_client->rtcp_worker(fd);
     while (1) {
         raw_offset = 0;
         raw_length = 0;
         memset(raw, '\0', sizeof(raw));

     read_pending:

         /* is buffer full ? */
         if (raw_length >= max_buf_size) {
             printf(">> RESETTING\n");

             if (raw_offset < raw_length - 1) {
                 int bytes = raw_length - raw_offset;

                 memset(raw_tmp, '\0', sizeof(raw_tmp));
                 memcpy(raw_tmp, raw + raw_offset, bytes);

                 memset(raw, '\0', sizeof(raw));
                 memcpy(raw, raw_tmp, bytes);

                printf("   Move:  %i\n", raw_length - raw_offset);
                raw_length = bytes;
                raw_offset = 0;
                if (raw[raw_offset] != 0x24) {
                    printf("MASTER CORRUPTION\n");
                }

            }
            else {
                raw_length = 0;
                memset(raw, '\0', sizeof(raw));
            }
         }

         /* read incoming data */
         printf(">> RECV: max %i bytes\n", max_buf_size - raw_length);
         r = recv(fd, (char*)(raw + raw_length), max_buf_size - raw_length, 0);
         printf(">> READ: %i (up to %i)\n", r, max_buf_size - raw_length);
         fflush(stdout);

         if (r <= 0) {
             if (errno == EAGAIN) {
                 //printf("Socket timeout, send out RTCP packets\n");
                 //goto read_pending;
             }

             _rtp_client->rtp_stats_print();
             printf(">> RTSP: Server closed connection!\n");

            /* cleanup */
            if (_streamer->get_params()->stream_fs_fd > 0) {
                close(_streamer->get_params()->stream_fs_fd);
                _streamer->get_params()->stream_fs_fd =0;
            }

            close(fd);
            close(_streamer->get_params()->stream_sock);
            close(_streamer->get_params()->stream_pipe[0]);
            close(_streamer->get_params()->stream_pipe[1]);
            exit(1);
            return -1;
         }

         raw_length += r;

        /* parse all data in our buffer */
        while (raw_offset < raw_length) {

            /* RTSP Interleaved header */
            if (raw[raw_offset] == 0x24) {
                channel = raw[raw_offset + 1];
                rtp_length  = (raw[raw_offset + 2] << 8 | raw[raw_offset + 3]);

                printf(">> RTSP Interleaved (offset = %i/%i)\n",
                       raw_offset, raw_length);
                printf("   Magic         : 0x24\n");
                printf("   Channel       : %i\n", channel);
                printf("   Payload Length: %i\n", rtp_length);

                if (raw_length > max_buf_size) {
                    printf("Error exception: raw_length = %i ; max_buf_size = %i\n",
                           raw_length, max_buf_size);
                    exit(EXIT_FAILURE);
                }

                /* RTSP header is 4 bytes, update the offset */
                raw_offset += 4;

                /* If no payload exists, continue with next bytes */
                if (rtp_length == 0) {
                    raw_offset -= 4;
                    goto read_pending;
                    continue;
                }

                if (rtp_length > (raw_length - raw_offset)) {
                    raw_offset -= 4;
                    printf("   ** Pending    : %u bytes\n",
                           rtp_length - (raw_length - raw_offset));
                          //[CH %i] PENDING: RTP_LENGTH=%i ; RAW_LENGTH=%i; RAW_OFFSET=%i\n",
                          // channel, rtp_length, raw_length, raw_offset);
                    if (send_rtcp == 1){
                        rtsp_rtcp_reports(fd);
                        send_rtcp = 0;
                    }

                    goto read_pending;
                }

                if (_rtp_client->get_stats()->rtp_received % 20) {
                    rtsp_rtcp_reports(fd);
                }

                /* RTCP data */
                if (channel >= 1) {
                    int idx;
                    int rtcp_count;
                    int size_RTCP_SR = 32;
                    int size_RTCP_SDES = 17;
                    int size_RTCP = 0;

                    struct rtcp_pkg *rtcp;

                    /* Decode RTCP packet(s) */
                    rtcp = _rtcp_client->rtcp_decode(raw + raw_offset, rtp_length, &rtcp_count);

                    if (rtcp_count >= 1 && rtcp[0].type == RTCP_SR) {
                        send_rtcp = 1;
                        //rtsp_rtcp_reports(fd);
                    }

                    raw_offset += rtp_length;
                    free(rtcp);
                    continue;
                }

                if (rtp_length == 0) {
                    continue;
                }

                /*
                 * Channel 0
                 * ---------
                 * If the channel is zero, the payload should contain RTP data,
                 * we need to identify the RTP header fields so later we can
                 * proceed to extract the H264 information.
                 */
                int offset;
                offset = _rtp_client->rtp_parse(raw + raw_offset, rtp_length);
                if (offset <= 0) {
                    raw_offset += rtp_length;
                }
                else {
                    raw_offset += offset;
                }

                if (send_rtcp == 1) {
                    //rtsp_rtcp_reports(fd);
                    send_rtcp = 0;
                }

                continue;
            }
            raw_offset++;
            continue;
        }
        continue;
    }
     close(_streamer->get_params()->stream_fs_fd);

    return 0;
}
