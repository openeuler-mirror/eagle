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

static struct Policy g_curPolicy;
static struct Policy g_lastPolicy;

int InitPolicyMgr(void)
{
    bzero(&g_curPolicy, sizeof(Policy));
    bzero(&g_lastPolicy, sizeof(Policy));
    int ret = InitPolicy(GetPolicyCfg()->policyFile, &g_curPolicy);
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCYMGR, "InitPolicy failed. ret: %d", ret);
    }
    memcpy(&g_lastPolicy, &g_curPolicy, sizeof(Policy));
    return SUCCESS;
}

int UpdatePolicy(void)
{
    // todo
}

Policy *GetCurPolicy(void)
{
    return (Policy *)&g_curPolicy;
}

Policy *GetLastPolicy(void)
{
    return (Policy *)&g_lastPolicy;
}