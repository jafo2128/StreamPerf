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

/* generic headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

/* local headers */
#include "MTDefine.h"
#include "MTBuffer.h"
#include "MTRtp.h"
#include "MTNetwork.h"
#include "MTStreamer.h"

#define PROTOCOL_PREFIX   "rtsp://"

mt_rtp::mt_rtp() {
    _streamer = new mt_streamer();
    memset(&_rtp_stats, 0 , sizeof(rtp_stats));
}

mt_rtp::~mt_rtp() {
    if(NULL != _streamer) {
        delete _streamer;
        _streamer = NULL;
    }
}

struct rtp_stats* mt_rtp::get_stats(){
    return &_rtp_stats;
};

int mt_rtp::rtp_connect(char *stream)
{
    char *host;
    char *sep;
    int len;
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
    host = (char *)malloc(len + 1);
    strncpy(host, stream + offset, len);
    host[len] = '\0';

    RTP_INFO("Connecting to host '%s' port %i...\n", host, 0);

    return net_udp_connect(host, 0);
}

unsigned int mt_rtp::rtp_parse(unsigned char *raw, unsigned int size)
{
    unsigned int raw_offset = 0;
    unsigned int rtp_length = size;
    unsigned int paysize;
    unsigned char payload[MAX_BUF_SIZE];
    struct rtp_header rtp_h;

    rtp_h.version = raw[raw_offset] >> 6;
    rtp_h.padding = CHECK_BIT(raw[raw_offset], 5);
    rtp_h.extension = CHECK_BIT(raw[raw_offset], 4);
    rtp_h.cc = raw[raw_offset] & 0xFF;

    /* next byte */
    raw_offset++;

    rtp_h.marker = CHECK_BIT(raw[raw_offset], 8);
    rtp_h.pt     = raw[raw_offset] & 0x7f;

    /* next byte */
    raw_offset++;

    /* Sequence number */
    rtp_h.seq = raw[raw_offset] * 256 + raw[raw_offset + 1];
    raw_offset += 2;

    /* time stamp */
    rtp_h.ts = \
        (raw[raw_offset    ] << 24) |
        (raw[raw_offset + 1] << 16) |
        (raw[raw_offset + 2] <<  8) |
        (raw[raw_offset + 3]);
    raw_offset += 4;

    /* ssrc / source identifier */
    rtp_h.ssrc = \
        (raw[raw_offset    ] << 24) |
        (raw[raw_offset + 1] << 16) |
        (raw[raw_offset + 2] <<  8) |
        (raw[raw_offset + 3]);
    raw_offset += 4;
    _rtp_stats.rtp_identifier = rtp_h.ssrc;

    /* Payload size */
    paysize = (rtp_length - raw_offset);

    memset(payload, '\0', sizeof(payload));
    memcpy(&payload, raw + raw_offset, paysize);

    /*
     * A new RTP packet has arrived, we need to pass the rtp_h struct
     * to the stats/context updater
     */
    rtp_stats_update(&rtp_h);

    /* Display RTP header info */
    printf("   >> RTP\n");
    printf("      Version     : %i\n", rtp_h.version);
    printf("      Padding     : %i\n", rtp_h.padding);
    printf("      Extension   : %i\n", rtp_h.extension);
    printf("      CSRC Count  : %i\n", rtp_h.cc);
    printf("      Marker      : %i\n", rtp_h.marker);
    printf("      Payload Type: %i\n", rtp_h.pt);
    printf("      Sequence    : %i\n", rtp_h.seq);
    printf("      Timestamp   : %u\n", rtp_h.ts);
    printf("      Sync Source : %u\n", rtp_h.ssrc);

    /*
     * NAL, first byte header
     *
     *   +---------------+
     *   |0|1|2|3|4|5|6|7|
     *   +-+-+-+-+-+-+-+-+
     *   |F|NRI|  Type   |
     *   +---------------+
     */
    int nal_forbidden_zero = CHECK_BIT(payload[0], 7);
    int nal_nri  = (payload[0] & 0x60) >> 5;
    int nal_type = (payload[0] & 0x1F);

    printf("      >> NAL\n");
    printf("         Forbidden zero: %i\n", nal_forbidden_zero);
    printf("         NRI           : %i\n", nal_nri);
    printf("         Type          : %i\n", nal_type);

    /* Single NAL unit packet */
    if (nal_type >= NAL_TYPE_SINGLE_NAL_MIN &&
        nal_type <= NAL_TYPE_SINGLE_NAL_MAX) {

        /* Write NAL header */
        _streamer->streamer_write_nal();

        /* Write NAL unit */
        _streamer->streamer_write(payload, sizeof(paysize));
    }

    /*
     * Agregation packet - STAP-A
     * ------
     * http://www.ietf.org/rfc/rfc3984.txt
     *
     * 0                   1                   2                   3
     * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |                          RTP Header                           |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |STAP-A NAL HDR |         NALU 1 Size           | NALU 1 HDR    |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |                         NALU 1 Data                           |
     * :                                                               :
     * +               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |               | NALU 2 Size                   | NALU 2 HDR    |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |                         NALU 2 Data                           |
     * :                                                               :
     * |                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |                               :...OPTIONAL RTP padding        |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     */
    else if (nal_type == NAL_TYPE_STAP_A) {
        uint8_t *q;
        uint16_t nalu_size;
        uint32_t nidx = 0;

        q = payload + 1;

        nidx = 0;
        while (nidx < paysize - 1) {
            /* write NAL header */
            _streamer->streamer_write_nal();

            /* get NALU size */
            nalu_size = (q[nidx] << 8) | (q[nidx + 1]);
            printf("nidx = %i ; NAL size = %i ; RAW offset = %i\n",
                   nidx, nalu_size, raw_offset);
            nidx += 2;

            /* write NALU size */
            _streamer->streamer_write(&nalu_size, 1);

            if (nalu_size == 0) {
                nidx++;
                continue;
            }

            /* write NALU data */
            _streamer->streamer_write(q + nidx, nalu_size);
            nidx += nalu_size;
        }
    } else if (nal_type == NAL_TYPE_FU_A) {
        printf("         >> Fragmentation Unit\n");

        uint8_t *q;
        q = payload;

        uint8_t h264_start_bit = q[1] & 0x80;
        uint8_t h264_end_bit   = q[1] & 0x40;
        uint8_t h264_type      = q[1] & 0x1F;
        uint8_t h264_nri       = (q[0] & 0x60) >> 5;
        uint8_t h264_key       = (h264_nri << 5) | h264_type;

        if (h264_start_bit) {
            /* write NAL header */
            _streamer->streamer_write_nal();

            /* write NAL unit code */
            _streamer->streamer_write(&h264_key, sizeof(h264_key));
        }
        _streamer->streamer_write(q + 2, paysize - 2);

        if (h264_end_bit) {
            /* nothing to do... */
        }
    } else if (nal_type == NAL_TYPE_UNDEFINED) {

    } else {
        printf("OTHER NAL!: %i\n", nal_type);
        raw_offset++;

    }
    raw_offset += paysize;

    if (rtp_h.seq > _rtp_stats.highest_seq) {
        _rtp_stats.highest_seq = rtp_h.seq;
    }

    this->rtp_stats_print();
    return raw_offset;
}

void mt_rtp::rtp_rtp2tval(unsigned int ts, struct timeval *tv)
{
    tv->tv_sec  = (ts * RTP_FREQ);
    tv->tv_usec = ((((ts % RTP_FREQ) / (ts / 8000))) * 125);
}

uint64_t mt_rtp::rtp_timeval_to_ntp(const struct timeval *tv)
{
    uint64_t msw;
    uint64_t lsw;

    /* 0x83AA7E80 is the number of seconds from 1900 to 1970 */
    msw = tv->tv_sec + 0x83AA7E80;
    lsw = (uint32_t)((double)tv->tv_usec*(double)(((uint64_t)1)<<32)*1.0e-6);

    return ((msw << 32) | lsw);
}

uint64_t mt_rtp::rtp_now()
{
    struct timeval tmp;
    gettimeofday(&tmp, NULL);
    return 0;
}

void mt_rtp::rtp_stats_reset()
{
    memset(&_rtp_stats, '\0', sizeof(struct rtp_stats));
}

/* Every time a RTP packet arrive, update the stats */
void mt_rtp::rtp_stats_update(struct rtp_header *rtp_h)
{
    uint32_t transit;
    int delta;
    struct timeval now;

    gettimeofday(&now, NULL);
    _rtp_stats.rtp_received++;

    /* Highest sequence */
    if (rtp_h->seq > _rtp_stats.highest_seq) {
        _rtp_stats.highest_seq = rtp_h->seq;
    }


    /* Update RTP timestamp */
    if (_rtp_stats.last_rcv_time.tv_sec == 0) {
        //_rtp_stats.rtp_ts = rtp_h->ts;
        _rtp_stats.first_seq = rtp_h->seq;
        //_rtp_stats.jitter = 0;
        //_rtp_stats.last_dlsr = 0;
        //_rtp_stats.rtp_cum_lost = 0;
        gettimeofday(&_rtp_stats.last_rcv_time, NULL);

        /* deltas
        int sec  = (rtp_h->ts / RTP_FREQ);
        int usec = (((rtp_h->ts % RTP_FREQ) / (RTP_FREQ / 8000))) * 125;
        _rtp_stats.ts_delta.tv_sec  = now.tv_sec - sec;
        _rtp_stats.ts_delta.tv_usec = now.tv_usec - usec;


        _rtp_stats.last_arrival = rtp_tval2rtp(_rtp_stats.ts_delta.tv_sec,
                                           _rtp_stats.ts_delta.tv_usec);
        _rtp_stats.last_arrival = rtp_tval2RTP(now);

    }
    else {*/
    }
        /* Jitter */
        transit = _rtp_stats.delay_snc_last_SR;
        //printf("TRANSIT!: %i\n", transit); exit(1);
        delta = transit - _rtp_stats.transit;
        _rtp_stats.transit = transit;
        if (delta < 0) {
            delta = -delta;
        }
        //printf("now = %i ; rtp = %i ; delta = %i\n",
        //       t, rtp_h->ts, delta);
        //_rtp_stats.jitter += delta - ((_rtp_stats.jitter + 8) >> 4);
        _rtp_stats.jitter += ((1.0/16.0) * ((double) delta - _rtp_stats.jitter));

        _rtp_stats.rtp_ts = rtp_h->ts;
        //}

    /* print the new stats */
    rtp_stats_print();
}

void mt_rtp::rtp_stats_print()
{
    printf(">> RTP Stats\n");
    printf("   First Sequence  : %u\n", _rtp_stats.first_seq);
    printf("   Highest Sequence: %u\n", _rtp_stats.highest_seq);
    printf("   RTP Received    : %u\n", _rtp_stats.rtp_received);
    printf("   RTP Identifier  : %u\n", _rtp_stats.rtp_identifier);
    printf("   RTP Timestamp   : %u\n", _rtp_stats.rtp_ts);
    printf("   Jitter          : %u\n", _rtp_stats.jitter);
    printf("   Last DLSR       : %i\n", _rtp_stats.last_dlsr);
}
