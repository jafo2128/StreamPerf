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

#ifndef MTPERFTASKUDP_H_INCLUDED
#define MTPERFTASKUDP_H_INCLUDED

#include "MTPerfTask.h"

class MTPerfTaskUDP : public MTPerfTask {
public:
    virtual int  netInit();
    virtual int  netListen();
    virtual int  netAccept();
    virtual int  netConnect();
    virtual int  netSend();
    virtual int  netRecv();
    virtual int doTask(void* args);
};

class MTPerfTaskUDPServer : public MTPerfTaskUDP {
public:
    MTPerfTaskUDPServer();
};

class MTPerfTaskUDPClient : public MTPerfTaskUDP {
public:
    MTPerfTaskUDPClient();
};


#endif // MTPERFTASKUDP_H_INCLUDED
