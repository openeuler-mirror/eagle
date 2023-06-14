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
#include "policyservice.h"

#include "common.h"
#include "log.h"
#include "config.h"
#include "devicectrl.h"
#include "pluginmgr.h"
#include "datacollect.h"
#include "policy.h"

static int g_hasRegistedToPapis = FALSE;

static int RegisterToPapis(void)
{
    // todo. register via powerapi.so
    g_hasRegistedToPapis = TRUE;
    return SUCCESS;
}

static void UnRegisterFromPapis(void)
{
    // todo.
    g_hasRegistedToPapis = FALSE;
}

static int LoadPolicyFile(void)
{
    int ret = InitPolicy(GetPolicyCfg()->policyFile);
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCYS, "InitPolicy failed. ret: %d", ret);
    }
    return SUCCESS;
}

static int BackupOriginalSysItems(void)
{
    // todo
    return SUCCESS;
}

static int RestoreOriginalSysItems(void)
{
    // todo
    return SUCCESS;
}

static int ExecutePolicy(void)
{
    // todo
    return SUCCESS;
}

static void EndPolicy(void)
{
    // todo
}

/* ****************public**************************************** */
int InitPolicyService(void)
{
    int ret = InitDevCtrl();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCYS, "InitDevCtrl failed. ret:%d", ret);
        return ret;
    }
    ret = InitPluginMgr();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCYS, "InitPluginMgr failed. ret:%d", ret);
        return ret;
    }
    ret = InitDataColl();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCYS, "InitDataColl failed. ret:%d", ret);
        return ret;
    }
    return SUCCESS;
}

int StartPolicyService(void)
{
    int ret = RegisterToPapis();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCYS, "RegisterToPapis failed. ret:%d", ret);
        return ret;
    }
    ret = LoadPolicyFile();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCYS, "LoadPolicyFile failed. ret:%d", ret);
        return ret;
    }
    ret = BackupOriginalSysItems();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCYS, "BackupOriginalItems failed. ret:%d", ret);
        return ret;
    }
    ret = ExecutePolicy();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCYS, "ExecutePolicy failed. ret:%d", ret);
        return ret;
    }
    return SUCCESS;
}

void StopPolicyService(void)
{
    EndPolicy();
    if (RestoreOriginalSysItems() != SUCCESS) {
        Logger(ERROR, MD_NM_PCYS, "RestoreOriginalSysItems failed");
    }

    UnRegisterFromPapis();
}

void CheckAndUpdatePolicy(void)
{
    // todo
}
