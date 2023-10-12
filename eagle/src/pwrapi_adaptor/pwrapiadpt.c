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
#include "public.h"
#include "powerapi.h"

int PwrapiSetLogCallback(void(LogCallback)(int level, const char *fmt, va_list vl))
{
    if (PWR_SetLogCallback(LogCallback) != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiRegister(void)
{
    if (PWR_Register() != PWR_SUCCESS) {
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
