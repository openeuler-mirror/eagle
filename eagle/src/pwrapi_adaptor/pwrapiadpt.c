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
 * Description: adaptor the powerapi
 * **************************************************************************** */

#include "pwrapiadpt.h"
#include <string.h>
#include <stdlib.h>
#include "public.h"
#include "pwrapic/powerapi.h"

#define ALL_PROCS_TAG "all"

static void (*g_loger)(int level, const char *fmt, va_list vl) = NULL;
static inline void PrintLog(int level, const char *fmt, ...)
{
    if (g_loger) {
        va_list vl;
        va_start(vl, fmt);
        g_loger(level, fmt, vl);
        va_end(vl);
    }
}

// public==============================================================================
int PwrapiSetLogCallback(void(LogCallback)(int level, const char *fmt, va_list vl))
{
    if (PWR_SetLogCallback(LogCallback) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    g_loger = LogCallback;
    return SUCCESS;
}

int PwrapiSetEventCallback(void EventCallback(const PWR_COM_EventInfo *eventInfo))
{
    if (PWR_SetEventCallback(EventCallback) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiRegister(void)
{
    if (PWR_Register() != PWR_SUCCESS) {
        PrintLog(ERROR, "Register to powerapi failed.");
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiUnRegister(void)
{
    (void)PWR_UnRegister();
    return SUCCESS;
}

int PwrapiRequestControlAuth(void)
{
    if (PWR_RequestControlAuth() != PWR_SUCCESS) {
         return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiReleaseControlAuth(void)
{
    if (PWR_ReleaseControlAuth() != PWR_SUCCESS) {
         return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiCpuSetFreqGovernor(const char gov[])
{
    if (PWR_CPU_SetFreqGovernor(gov) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiCpuSetFreqGovAttribute(const char *attrName, const char *attrValue)
{
    if (!attrName || !attrValue) {
        return ERR_NULL_POINTER;
    }
    if (strlen(attrName) == 0) {
        return ERR_INVALIDE_PARAM;
    }
    PWR_CPU_FreqGovAttr govAttr = {0};
    strncpy(govAttr.attr.key, attrName, PWR_MAX_ELEMENT_NAME_LEN - 1);
    strncpy(govAttr.attr.value, attrValue, PWR_MAX_VALUE_LEN - 1);
    if (PWR_CPU_SetFreqGovAttr(&govAttr) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiCpuGetIdleGovernor(char gov[], uint32_t size)
{
    if (PWR_CPU_GetIdleGovernor(gov, size) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiCpuSetIdleGovernor(const char gov[])
{
    if (PWR_CPU_SetIdleGovernor(gov) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrProcSetWattFirstDomain(int cpuId)
{
    if (PWR_PROC_SetWattFirstDomain(cpuId) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrProcSetWattState(int state)
{
    if (PWR_PROC_SetWattState(state) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiProcSetWattAttr(const SchedServicePcy *schedPcy)
{
    if (!schedPcy) {
        return ERR_NULL_POINTER;
    }
    PWR_PROC_WattAttrs wattAttrs = {0};
    wattAttrs.scaleThreshold = schedPcy->wattThreshold;
    wattAttrs.scaleInterval = schedPcy->wattInterval;
    wattAttrs.domainMask = schedPcy->wattMask;
    if (PWR_PROC_SetWattAttrs(&wattAttrs) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiProcAddWattProcs(const char *keyWords)
{
    const char *kw = NULL;
    if (keyWords && strcmp(keyWords, ALL_PROCS_TAG) != 0) {
        kw = keyWords;
    }
    pid_t procs[PWR_MAX_PROC_NUM] = {0};
    uint32_t num = PWR_MAX_PROC_NUM;
    if (PWR_PROC_QueryProcs(kw, procs, &num) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    if (num != 0) {
        if (PWR_PROC_AddWattProcs(procs, num) != PWR_SUCCESS) {
            return ERR_INVOKE_PWRAPI_FAILED;
        }
    }
    return SUCCESS;
}

int PwrProcSetSmartGridState(int state)
{
    if (PWR_PROC_SetSmartGridState(state) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiProcAddSmartGridProcs(const char *keyWords)
{
    const char *kw = NULL;
    if (keyWords && strcmp(keyWords, ALL_PROCS_TAG) != 0) {
        kw = keyWords;
    }
    size_t size = sizeof(PWR_PROC_SmartGridProcs) + PWR_MAX_PROC_NUM * sizeof(pid_t);
    PWR_PROC_SmartGridProcs *sgProcs = (PWR_PROC_SmartGridProcs *)malloc(size);
    if (!sgProcs) {
        return ERR_SYS_EXCEPTION;
    }
    bzero(sgProcs, size);
    sgProcs->level = PWR_PROC_SG_LEVEL_0; // vip
    sgProcs->procNum = PWR_MAX_PROC_NUM;

    if (PWR_PROC_QueryProcs(kw, sgProcs->procs, &(sgProcs->procNum)) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    if (sgProcs->procNum != 0) {
        if (PWR_PROC_SetSmartGridLevel(sgProcs) != PWR_SUCCESS) {
            return ERR_INVOKE_PWRAPI_FAILED;
        }
    }
    return SUCCESS;
}

int PwrapiProcSetSmartGridGov(const SchedServicePcy *schedPcy)
{
    PWR_PROC_SmartGridGov sgGov = {0};
    sgGov.sgAgentState = schedPcy->sgGovEnable;
    strncpy(sgGov.sgLevel0Gov, schedPcy->sgVipGov, sizeof(sgGov.sgLevel0Gov));
    strncpy(sgGov.sgLevel1Gov, schedPcy->sgLev1Gov, sizeof(sgGov.sgLevel1Gov));
    if (PWR_PROC_SetSmartGridGov(&sgGov) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiProcSetServiceState(PWR_PROC_ServiceState *sState)
{
    if (PWR_PROC_SetServiceState(sState) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}
int PwrapiProcGetServiceState(PWR_PROC_ServiceStatus *sStatus)
{
    if (PWR_PROC_GetServiceState(sStatus) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}