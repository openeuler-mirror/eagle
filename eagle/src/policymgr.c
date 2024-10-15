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
 * Create: 2023-07-20
 * Description: loading config file and manager all config items for the PowerAPI service
 * **************************************************************************** */

#include "policymgr.h"
#include <string.h>
#include "log.h"
#include "config.h"
#include "utils.h"

static struct Policy g_curPolicy;
static struct Policy g_lastPolicy;
static char pcyFileMd5[MD5_LEN] = {0};

int InitPolicyMgr(void)
{
    bzero(&g_curPolicy, sizeof(Policy));
    bzero(&g_lastPolicy, sizeof(Policy));
    int ret = CreatePolicy(GetPolicyCfg()->policyFile, &g_curPolicy);
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCYMGR, "CreatePolicy failed. ret: %d", ret);
    }
    memcpy(&g_lastPolicy, &g_curPolicy, sizeof(Policy));
    GetMd5(GetPolicyCfg()->policyFile, pcyFileMd5);
    if (strlen(pcyFileMd5) == 0) {
        Logger(ERROR, MD_NM_PCYMGR, "Get initial md5 of policy file failed.");
        return FAILED;
    }
    return SUCCESS;
}

// return TRUE if the policy file or the any plugin lib modified.
int UpdatePolicy(void)
{
    char curMd5[MD5_LEN] = {0};
    GetMd5(GetPolicyCfg()->policyFile, curMd5);
    if (strlen(curMd5) == 0 || (strcmp(curMd5, pcyFileMd5) == 0 && !PluginLibModified(&g_curPolicy))) {
        return FALSE;
    }
    strncpy(pcyFileMd5, curMd5, MD5_LEN);
    Logger(INFO, MD_NM_PCYMGR, "Policy file or plugin libs modified. path: %s", GetPolicyCfg()->policyFile);
    struct Policy pcy = {0};
    int ret = CreatePolicy(GetPolicyCfg()->policyFile, &pcy);
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCYMGR, "Parse policy file failed. path:%s ret: %d",
            GetPolicyCfg()->policyFile, ret);
        return FALSE;
    }
    (void)UpdateModifiedFlag(&g_curPolicy, &pcy);
    memcpy(&g_lastPolicy, &g_curPolicy, sizeof(Policy));
    memcpy(&g_curPolicy, &pcy, sizeof(Policy));
    return TRUE;
}

Policy *GetCurPolicy(void)
{
    return (Policy *)&g_curPolicy;
}

Policy *GetLastPolicy(void)
{
    return (Policy *)&g_lastPolicy;
}
