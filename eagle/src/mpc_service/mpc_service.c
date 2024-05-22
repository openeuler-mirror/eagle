/* *****************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024 All rights reserved.
 * eagle licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: heppen
 * Create: 2024-05-20
 * Description: provide intelligent mpc service.
 * **************************************************************************** */

#include "mpc_service.h"
#include <stdio.h>
#include <string.h>
#include "public.h"
#include "policydt.h"
#include "pwrapiadpt.h"

static void DefaultLogCallback(int level, const char *usInfo, const char *fmt, va_list vl)
{
    printf(fmt);
}

static void (*g_log_callback)(int level, const char *usInfo, const char *fmt, va_list vl)
    = DefaultLogCallback;
static char g_id[MAX_VALUE] = {0};

static PWR_PROC_SERVICE_STATUS mpc_origin_stat = PWR_PROC_SRV_ST_UNKNOWN;

static inline void SrvLog(int level, const char *fmt, ...)
{
    if (g_log_callback) {
        va_list vl;
        va_start(vl, fmt);
        g_log_callback(level, g_id, fmt, vl);
        va_end(vl);
    }
}

static inline int IsServiceStarted(const PWR_PROC_SERVICE_STATUS state)
{
    return ((state == PWR_PROC_SRV_ST_ACTIVATING) || (state == PWR_PROC_SRV_ST_RUNNING) || (state == PWR_PROC_SRV_ST_WAITING));
}

static int SetMpcState(const int state)
{
    PWR_PROC_ServiceStatus mpc_status = {0};
    mpc_status.name = PWR_PROC_SERVICE_MPCTOOL;

    int ret = PwrapiProcGetServiceState(&mpc_status);
    if (ret != SUCCESS) {
        SrvLog(ERROR, "Failed to get mpc service status, ret is %d.", ret);
        return ERR_INVOKE_PWRAPI_FAILED;
    }

    PWR_PROC_ServiceState mpc_state = {0};
    mpc_state.name = PWR_PROC_SERVICE_MPCTOOL;

    if (state == PCY_ENABLE) {
        switch (mpc_status.status) {
        case PWR_PROC_SRV_ST_INACTIVE: // only start mpc when mpc is inactive
            mpc_state.state = 1;
            ret = PwrapiProcSetServiceState(&mpc_state);
            break;
        default:
            SrvLog(INFO, "mpc service is not inactive , not need to enable.");
            break;
        }
    } else { // stop mpc service
        switch (mpc_status.status) {
        case PWR_PROC_SRV_ST_INACTIVE:
        case PWR_PROC_SRV_ST_EXITED:
        case PWR_PROC_SRV_ST_FAILED:
        case PWR_PROC_SRV_ST_UNKNOWN:
            SrvLog(INFO, "mpc service is already inactive, not need to disable.");
            break;
        default:
            mpc_state.state = 0;
            ret = PwrapiProcSetServiceState(&mpc_state);
            break;
        }
    }
    return ret;
}

static int ParsePolicy(const MpcServicePcy *mpcPcy)
{
    return SetMpcState(mpcPcy->enableMpc);

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
    SrvLog(INFO, "mpc_service initialized.");
    return SUCCESS;
}

int SRV_Start(void* pcy)
{
    if (!pcy) {
        SrvLog(ERROR, "mpc service start failed, pcy is null");
        return ERR_NULL_POINTER;
    }

    PWR_PROC_ServiceStatus mpc_status = {0};
    mpc_status.name = PWR_PROC_SERVICE_MPCTOOL;
    int ret = PwrapiProcGetServiceState(&mpc_status);
    if (ret != SUCCESS) {
        SrvLog(ERROR, "Failed to get mpc service status, ret is %d.", ret);
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    // there is no mpc service，nothing to do
    if (mpc_status.status == PWR_PROC_SRV_ST_UNKNOWN) {
        SrvLog(ERROR, "There is no mpc service.");
        return ERR_NO_SERVICE_AVAILABLE;
    }

    mpc_origin_stat = mpc_status.status;

    return ParsePolicy((MpcServicePcy*)pcy);
}

int SRV_Update(void* pcy)
{
    if (!pcy) {
        SrvLog(ERROR, "mpc service update failed, pcy is null");
        return ERR_NULL_POINTER;
    }
    return ParsePolicy((MpcServicePcy*)pcy);
}

int SRV_Looper(void)
{
    // todo
    return SUCCESS;
}

int SRV_Stop(void)
{
    if (mpc_origin_stat == PWR_PROC_SRV_ST_UNKNOWN) {
        SrvLog(INFO, "There is no mpc service");
        return SUCCESS;
    }
    return SetMpcState(IsServiceStarted(mpc_origin_stat));
}

int SRV_Uninit(void)
{
    return SUCCESS;
}