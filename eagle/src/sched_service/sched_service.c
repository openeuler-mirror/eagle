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
#include <string.h>
#include "public.h"
#include "policydt.h"
#include "pwrapiadpt.h"

static SchedServicePcy *g_schedPcy = NULL;

static void DefaultLogCallback(int level, const char *usInfo, const char *fmt, va_list vl)
{
    printf(fmt);
}

static void (*g_log_callback)(int level, const char *usInfo, const char *fmt, va_list vl)
    = DefaultLogCallback;
static char g_id[MAX_VALUE] = {0};

static inline void SrvLog(int level, const char *fmt, ...)
{
    if (g_log_callback) {
        va_list vl;
        va_start(vl, fmt);
        g_log_callback(level, g_id, fmt, vl);
        va_end(vl);
    }
}

static int ParsePolicy(const SchedServicePcy *schedPcy)
{
    int ret = SUCCESS;

    (void)PwrProcSetWattFirstDomain(schedPcy->wattFirstDomain);
    (void)PwrProcSetWattState(schedPcy->wattEnable);
    if (schedPcy->wattEnable) {
        (void)PwrapiProcSetWattAttr(schedPcy);
        (void)PwrapiProcAddWattProcs(schedPcy->wattProcs);
    }

    (void)PwrProcSetSmartGridState(schedPcy->sgEnable);
    if (schedPcy->sgEnable) {
        (void)PwrapiProcAddSmartGridProcs(schedPcy->sgVipProcs);
        (void)PwrapiProcSetSmartGridGov(schedPcy);
    }

    return ret;
}

// public ===============================================================================
int SRV_SetLogCallback(void(LogCallback)(int, const char *, const char *, va_list),
    const char *usrInfo)
{
    if (LogCallback) {
        g_log_callback = LogCallback;
        if (usrInfo) {
            strncpy(g_id, usrInfo, sizeof(g_id) - 1);
        }
        return SUCCESS;
    }
    return ERR_NULL_POINTER;
}

int SRV_Init(void)
{
    SrvLog(INFO, "sched_service initialized.");
    return SUCCESS;
}

int SRV_Start(void* pcy)
{
    if (!pcy) {
        SrvLog(ERROR, "SRV_Start, pcy is null");
        return ERR_NULL_POINTER;
    }
    g_schedPcy = (SchedServicePcy*)pcy;
    return ParsePolicy((SchedServicePcy*)pcy);
}

int SRV_Update(void* pcy)
{
    if (!pcy) {
        SrvLog(ERROR, "SRV_Update, pcy is null");
        return ERR_NULL_POINTER;
    }
    g_schedPcy = (SchedServicePcy*)pcy;
    return ParsePolicy((SchedServicePcy*)pcy);
}

int SRV_Looper(void)
{
    // Keep adding task to the watt task queue.
    if (g_schedPcy->wattEnable) {
        (void)PwrapiProcAddWattProcs(g_schedPcy->wattProcs);
    }

    if (g_schedPcy->sgEnable) {
        (void)PwrapiProcAddSmartGridProcs(g_schedPcy->sgVipProcs);
    }
    return SUCCESS;
}

int SRV_Stop(int mode)
{
    // todo
    return SUCCESS;
}

int SRV_Uninit(void)
{
    g_schedPcy = NULL;
    return SUCCESS;
}