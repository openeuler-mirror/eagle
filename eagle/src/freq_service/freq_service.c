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
 * Create: 2023-08-17
 * Description: provide frequency scaling service
 * **************************************************************************** */

#include "freq_service.h"
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

static char freq_origin_gov[PWR_MAX_ELEMENT_NAME_LEN] = {0};

static inline void SrvLog(int level, const char *fmt, ...)
{
    if (g_log_callback) {
        va_list vl;
        va_start(vl, fmt);
        g_log_callback(level, g_id, fmt, vl);
        va_end(vl);
    }
}

#define CPUFREQ_ATTR_SAMPLING_RATE "sampling_rate"
#define CPUFREQ_ATTR_PERF_LOSS_RATE "perf_loss_rate"

static int ParsePolicy(const FreqServicePcy *freqPcy)
{
    int ret = SUCCESS;
    if (strcmp(freqPcy->freqGov, "") != 0) {
        ret = PwrapiCpuSetFreqGovernor(freqPcy->freqGov);
        if ( ret != SUCCESS)
        {
            SrvLog(ERROR, "freq_service. set cpufreq gov to %s failed.", freqPcy->freqGov);
            return ret;
        }
    }
    if (freqPcy->samplingRate != 0) {
        char str_sr[MAX_VALUE] = {0};
        if (sprintf(str_sr, "%d", freqPcy->samplingRate) < 0) {
            return ERR_SYS_EXCEPTION;
        }
        ret = PwrapiCpuSetFreqGovAttribute(CPUFREQ_ATTR_SAMPLING_RATE, str_sr);
        if ( ret != SUCCESS)
        {
            SrvLog(ERROR, "freq_service. set gov sampling rate to %s failed.", str_sr);
            return ret;
        }
    }
    if (strcmp(freqPcy->freqGov, "seep") == 0 && freqPcy->perfLossRate != -1) {
        char str_plr[MAX_VALUE] = {0};
        if (sprintf(str_plr, "%d", freqPcy->perfLossRate) < 0) {
            return ERR_SYS_EXCEPTION;
        }
        ret = PwrapiCpuSetFreqGovAttribute(CPUFREQ_ATTR_PERF_LOSS_RATE, str_plr);
        if ( ret != SUCCESS)
        {
            SrvLog(ERROR, "freq_service. set gov perfLossRate to %s failed.", str_plr);
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
    SrvLog(INFO, "freq_service initialized.");
    int ret = PwrapiCpuGetFreqGovernor(freq_origin_gov, PWR_MAX_ELEMENT_NAME_LEN);
    if (ret != SUCCESS) {
        SrvLog(ERROR, "Failed to get origin freq governor, ret is %d.", ret);
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
    return ParsePolicy((FreqServicePcy*)pcy);
}

int SRV_Update(void* pcy)
{
    if (!pcy) {
        SrvLog(ERROR, "SRV_Update, pcy is null");
        return ERR_NULL_POINTER;
    }
    return ParsePolicy((FreqServicePcy*)pcy);
}

int SRV_Looper(void)
{
    // todo
    return SUCCESS;
}

int SRV_Stop(int mode)
{
    (void)mode; // reserved
    if (strlen(freq_origin_gov) == 0) {
        SrvLog(INFO, "There is no origin cpu freq governor");
        return SUCCESS;
    }
    int ret = PwrapiCpuSetFreqGovernor(freq_origin_gov);
    if (ret != SUCCESS) {
        SrvLog(ERROR, "Failed to set origin freq governor, ret is %d.", ret);
    }
    return ret;
}

int SRV_Uninit(void)
{
    return SUCCESS;
}
