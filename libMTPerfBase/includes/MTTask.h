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

#ifndef MTTASK_H_INCLUDED
#define MTTASK_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

typedef void (*TASK_FUNC)(void *, uint32_t);

typedef enum {
    PRIORITY_FIFO     = 0,
    PRIORITY_NORMAL   = 1,
    PRIORITY_HIGH     = 2,
} TASK_PRIORITY;

class MTTask {
public:
    uint32_t      mTid;
    TASK_PRIORITY mPriority;
    TASK_FUNC     mTaskFunc;
    void          *mArgs;
    uint32_t      mFlags;
    bool          mCancel;
    char*         mTagName;
public:
    MTTask();
    virtual ~MTTask();
    virtual int  doTask(void* args);
    virtual void cancel();
};

#endif // MTTASK_H_INCLUDED


