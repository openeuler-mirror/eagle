/* *****************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022 All rights reserved.
 * eagle licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: queyanwen
 * Create: 2022-06-23
 * Description: loading config file and manager all config items for the PowerAPI service
 * **************************************************************************** */
#include "eaglecore.h"

#include "common.h"
#include "log.h"
#include "config.h"
#include "servicemgr.h"
#include "policymgr.h"
#include "pwrapiadpt.h"


static int g_hasRegistedToPapis = FALSE;

static void PwrapiLogCallback(int level, const char *fmt, va_list vl)
{
    char message[MAX_LINE_NUM] = {0};

    if (vsnprintf(message, sizeof(message) - 1, fmt, vl) < 0) {
        return;
    }
    Logger(level, MD_NM_PWRAPI, message);
}

static void EventCallback(const PWR_COM_EventInfo *eventInfo)
{
    if (!eventInfo) {
        return;
    }
    Logger(INFO, MD_NM_ECORE, "Get event from powerapi. type:%d.", eventInfo->eventType);
    switch (eventInfo->eventType) {
        case PWR_COM_EVTTYPE_AUTH_RELEASED:
            // As the config auth has been released, eagle couldn't restore the system configuration.
            StopEagleSystem(EXIT_MODE_KEEP_STATUS);
            break;
        case PWR_COM_EVTTYPE_CRED_FAILED:
        default:
            break;
    }
}

static int RegisterToPapis(void)
{
    PwrapiSetLogCallback(PwrapiLogCallback);
    PwrapiSetEventCallback(EventCallback);
    int ret = PwrapiRegister();
    if (ret != SUCCESS) {
        return ret;
    }
    ret = PwrapiRequestControlAuth();
    if (ret != SUCCESS) {
        return ret;
    }
    g_hasRegistedToPapis = TRUE;
    return SUCCESS;
}

static void UnRegisterFromPapis(void)
{
    PwrapiReleaseControlAuth();
    PwrapiUnRegister();
    g_hasRegistedToPapis = FALSE;
}

static int BackupOriginalSettings(void)
{
    // todo
    return SUCCESS;
}

static int RestoreOriginalSettings(void)
{
    // todo
    return SUCCESS;
}


/* ****************public**************************************** */
int InitEagleSystem(void)
{
    int ret = InitPolicyMgr();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_ECORE, "InitPolicyMgr failed. ret:%d", ret);
        return ret;
    }
    ret = InitServiceMgr();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_ECORE, "InitServiceMgr failed. ret:%d", ret);
        return ret;
    }
    Logger(INFO, MD_NM_ECORE, "InitEagleSystem succeed.");
    return SUCCESS;
}

int StartEagleSystem(void)
{
    int ret = RegisterToPapis();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_ECORE, "RegisterToPapis failed. ret:%d", ret);
        return ret;
    }

    ret = BackupOriginalSettings();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_ECORE, "BackupOriginalItems failed. ret:%d", ret);
        return ret;
    }
    ret = StartServices();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_ECORE, "StartServices failed. ret:%d", ret);
        return ret;
    }
    Logger(INFO, MD_NM_ECORE, "StartEagleSystem succeed.");
    return SUCCESS;
}

void StopEagleSystem(int mode)
{
    StopServices(mode);
    if (RestoreOriginalSettings() != SUCCESS) {
        Logger(ERROR, MD_NM_ECORE, "RestoreOriginalSettings failed");
    }

    UnRegisterFromPapis();
}

void CheckAndUpdatePolicy(void)
{
    if(UpdatePolicy()) {
        UpdateServices();
    }
}
void TriggerTimer(void)
{
    TriggerTimerForServices();
}
