/* *****************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023 All rights reserved.
 * eagle licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: queyanwen
 * Create: 2023-06-28
 * Description: provide intelligent scheduling service
 * **************************************************************************** */

#include "sched_service.h"
#include <stdio.h>
#include "public.h"

static void DefaultLogCallback(int level, const char *usInfo, const char *fmt, va_list vl)
{
    printf(fmt);
}

static void (*g_log_callback)(int level, const char *usInfo, const char *fmt, va_list vl)
    = DefaultLogCallback;
static const char *g_id = NULL;

static inline void SrvLog(int level, const char *fmt, ...)
{
    if (g_log_callback) {
        va_list vl;
        va_start(vl, fmt);
        g_log_callback(level, g_id, fmt, vl);
        va_end(vl);
    }
}

int SRV_SetLogCallback(void(LogCallback)(int, const char *, const char *, va_list),
    const char *usrInfo)
{
    if (LogCallback) {
        g_log_callback = LogCallback;
        g_id = usrInfo;
        return SUCCESS;
    }
    return ERR_NULL_POINTER;
}

int SRV_Init(void)
{
    // todo
    SrvLog(ERROR, "SRV_Init called. d:%d", 0);
}

int SRV_Start(void* pcy)
{
    // todo
    SrvLog(ERROR, "SRV_Start called. d:%d", 1);
}

int SRV_Update(void* pcy)
{
    // todo
    SrvLog(ERROR, "SRV_Update called. d:%d", 2);
}

int SRV_Stop(void)
{
    // todo
    SrvLog(ERROR, "SRV_Stop called. d:%d", 3);
}

int SRV_Uninit(void)
{
    // todo
    SrvLog(ERROR, "SRV_Uninit called. d:%d", 4);
}