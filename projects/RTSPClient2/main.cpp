/*
 * MTPerfRTSP
 * an implemention of RTSPparam_client for performance test
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
#include <getopt.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

/* local headers */
#include "MTRtsp.h"
#include "MTNetwork.h"
//#include "MTStreamer.h"

void help(int status)
{
    printf("Usage: h264dec [-v] [-V] [-d FILENAME] -n CHANNEL_NAME -s rtsp://stream\n\n");
    printf("  -d, --dump=FILENAME      Dump H264 video data to a file\n");
    printf("  -s, --stream=URL         RTSP media stream address\n");
    printf("  -n, --name=CHANNEL_NAME  Name for local channel identifier\n");
	printf("  -h, --help               print this help\n");
	printf("  -v, --version            print version number\n");
	printf("  -V, --verbose            enable verbose mode\n");
	printf("\n");
	exit(status);
}

void banner()
{
    printf("MTRTSPparam_client v: %s\n\n", VERSION);
}

int main(int argc, char **argv)
{
     int opt;

     struct rtsp_params param_client;
     memset(&param_client, 0, sizeof(struct rtsp_params));
     param_client.stream_port = RTSP_PORT;
     param_client.client_port = RTSP_CLIENT_PORT;
     param_client.stream_dump = NULL;

     static const struct option long_opts[] = {
         { "stdout",  no_argument      , NULL, 'o' },
         { "dump",    required_argument, NULL, 'd' },
         { "stream",  required_argument, NULL, 's' },
         { "port",    required_argument, NULL, 'p' },
         { "name",    required_argument, NULL, 'n' },
         { "version", no_argument,       NULL, 'v' },
         { "verbose", no_argument,       NULL, 'V' },
         { "help",    no_argument,       NULL, 'h' },
         { NULL, 0, NULL, 0 }
     };

     while ((opt = getopt_long(argc, argv, "od:s:p:n:vVh",
                               long_opts, NULL)) != -1) {
         switch (opt) {
         case 'o':
             param_client.opt_stdout = 1;
             break;
         case 'd':
             param_client.stream_dump = strdup(optarg);
             break;
         case 's':
             param_client.opt_stream = strdup(optarg);
             break;
         case 'p':
             param_client.client_port = atoi(optarg);
             break;
         case 'n':
             param_client.opt_name = strdup(optarg);
             break;
         case 'v':
             banner();
             exit(EXIT_SUCCESS);
         case 'V':
             param_client.opt_verbose = 1;
             break;
         case 'h':
             help(EXIT_SUCCESS);
         case '?':
             help(EXIT_FAILURE);
         }
     }

     if (!param_client.opt_stream || strncmp(param_client.opt_stream, PROTOCOL_PREFIX,
                                sizeof(PROTOCOL_PREFIX) - 1) != 0) {
         printf("Error: Invalid stream input.\n\n");
         help(EXIT_FAILURE);
     }

     if (!param_client.opt_name) {
         printf("Error: Local channel name not specified.\n\n");
         help(EXIT_FAILURE);
     }

     /* RTSP loop */
     while (1) {
         open_net_driver();
         mt_rtsp* client = new mt_rtsp(&param_client);
         client->rtsp_loop();
         close_net_driver();
         printf("[ERROR] RTSP Loop stopped, waiting 5 seconds...\n");
         usleep(5000*1000);
         exit(1);
     }

     return 0;
}
