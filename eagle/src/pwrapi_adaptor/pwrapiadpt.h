/* *****************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * eagle licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: queyanwen, wuhaotian
 * Create: 2023-06-28
 * Description: adaptor the powerapi
 * **************************************************************************** */
#ifndef EAGLE_PWRAPI_ADAPTOR_H__
#define EAGLE_PWRAPI_ADAPTOR_H__

#include <stdio.h>
#include "policydt.h"
#include "pwrapic/pwrdata.h"

#ifdef __cplusplus
extern "C" {
#endif

int PwrapiSetLogCallback(void(LogCallback)(int level, const char *fmt, va_list vl));
int PwrapiSetEventCallback(void EventCallback(const PWR_COM_EventInfo *eventInfo));
int PwrapiRegister(void);
int PwrapiUnRegister(void);
int PwrapiRequestControlAuth(void);
int PwrapiReleaseControlAuth(void);
int PwrapiCpuGetFreqGovernor(char gov[], uint32_t size);
int PwrapiCpuSetFreqGovernor(const char gov[]);
int PwrapiCpuSetFreqGovAttribute(const char *attrName, const char *attrValue);
int PwrapiCpuGetIdleGovernor(char gov[], uint32_t size);
int PwrapiCpuSetIdleGovernor(const char gov[]);

// PROC
int PwrProcSetWattFirstDomain(int cpuId);
int PwrProcGetWattState(int *state);
int PwrProcSetWattState(int state);
int PwrProcGetWattAttrs(SchedServicePcy *schedPcy);
int PwrProcSetWattAttr(const SchedServicePcy *schedPcy);
int PwrProcAddWattProcs(const char *keyWords);
int PwrProcSetSmartGridState(int state);
int PwrProcGetSmartGridState(int *state);
int PwrProcAddSmartGridProcs(const char *keyWords);
int PwrProcGetSmartGridGov(SchedServicePcy *schedPcy);
int PwrProcSetSmartGridGov(const SchedServicePcy *schedPcy);
int PwrProcGetServiceState(PWR_PROC_ServiceStatus *sStatus);
int PwrProcSetServiceState(PWR_PROC_ServiceState *sState);

#ifdef __cplusplus
}
#endif

#endif
