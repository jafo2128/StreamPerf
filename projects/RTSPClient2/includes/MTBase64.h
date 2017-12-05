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

#ifndef _MT_RTSP_BASE64_H_
#define _MT_RTSP_BASE64_H_

#include <stddef.h>
#include "MTDefine.h"

uint8 *base64_decoder(const uint8 *src, size_t len, size_t *out_len);

uint8 *base64_encoder(const uint8 *src, size_t len, size_t *out_len);

#endif /*_MT_RTSP_BASE64_H_*/
