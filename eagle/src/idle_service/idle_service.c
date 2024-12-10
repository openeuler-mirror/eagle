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
 * Author: wuhaotian
 * Create: 2024-05-23
 * Description: provide cpu idle service
 * **************************************************************************** */

#include "idle_service.h"
#include <stdio.h>
#include <string.h>
#include "public.h"
#include "policydt.h"
#include "pwrapiadpt.h"

static void DefaultLogCallback(int level, const char *usInfo, const char *fmt, va_list vl)
{
    (void)level;
    (void)usInfo;

    vprintf(fmt, vl);
}

static void (*g_log_callback)(int level, const char *usInfo, const char *fmt, va_list vl)
    = DefaultLogCallback;
static char g_id[MAX_VALUE] = {0};

static char idle_origin_gov[PWR_MAX_ELEMENT_NAME_LEN] = {0};

static inline void SrvLog(int level, const char *fmt, ...)
{
    if (g_log_callback) {
        va_list vl;
        va_start(vl, fmt);
        g_log_callback(level, g_id, fmt, vl);
        va_end(vl);
    }
}

static int ParsePolicy(const IdleServicePcy *idlePcy)
{
    int ret = SUCCESS;
    if (strcmp(idlePcy->idleGov, "") != 0) {
        ret = PwrapiCpuSetIdleGovernor(idlePcy->idleGov);
        if ( ret != SUCCESS)
        {
            SrvLog(ERROR, "idle_service. set cpuidle gov to %s failed.", idlePcy->idleGov);
            return ret;
        }
    }
    return SUCCESS;
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
    SrvLog(INFO, "idle_service initialized.");

    int ret = PwrapiCpuGetIdleGovernor(idle_origin_gov, PWR_MAX_ELEMENT_NAME_LEN);
    if(ret != SUCCESS) {
        SrvLog(ERROR, "Failed to get origin idle governor, ret is %d.", ret);
        return ERR_INVOKE_PWRAPI_FAILED;
    }

    return SUCCESS;
}

int SRV_Start(void* pcy)
{
    if (!pcy) {
        SrvLog(ERROR, "SRV_Start, pcy is null");
        return ERR_NULL_POINTER;
    }

    return ParsePolicy((IdleServicePcy*)pcy);
}

int SRV_Update(void* pcy)
{
    if (!pcy) {
        SrvLog(ERROR, "SRV_Update, pcy is null");
        return ERR_NULL_POINTER;
    }
    return ParsePolicy((IdleServicePcy*)pcy);
}

int SRV_Looper(void)
{
    // todo
    return SUCCESS;
}

int SRV_Stop(void)
{
    if (strlen(idle_origin_gov) == 0) {
        SrvLog(INFO, "There is no origin cpu idle governor");
        return SUCCESS;
    }
    int ret = PwrapiCpuSetIdleGovernor(idle_origin_gov);
    if (ret != SUCCESS) {
        SrvLog(ERROR, "Failed to set origin idle governor, ret is %d.", ret);
    }
    return ret;
}

int SRV_Uninit(void)
{
    return SUCCESS;
}
