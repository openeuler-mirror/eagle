/* *****************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * eagle licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: luocaimin
 * Create: 2022-03-29
 * Description: provide const value definition
 * **************************************************************************** */
#ifndef EAGLE_COMMON_H__
#define EAGLE_COMMON_H__

#include "public.h"

// Define module name
#define MD_NM_MAN "MAIN"
#define MD_NM_LOG "LOG"
#define MD_NM_CFG "CONFIG"
#define MD_NM_ECORE "EAGLE_CORE"
#define MD_NM_DCTRL "DEV_CTRL"
#define MD_NM_SVRMGR "SERVICE_MGR"
#define MD_NM_PCYMGR "POLICY_MGR"
#define MD_NM_DCOLL "DATA_COLL"
#define MD_NM_SVR "SERVICE"
#define MD_NM_PCY "POLICY"
#define MD_NM_PWRAPI "PWRAPI"

// Define configuration section name
#define CFG_NM_LOG "log"
#define CFG_NM_PCY "policy"
#define CFG_NM_TMR "timer"

// Define cfg item name
// log
#define CFG_IT_FLS "file_size"
#define CFG_IT_CNT "cmp_cnt"
#define CFG_IT_LGV "log_level"
#define CFG_IT_LGP "log_path"
#define CFG_IT_BKP "bak_log_path"
#define CFG_IT_PFX "log_pfx"
// policy
#define CFG_IT_PCY "policy"
#define CFG_IT_PPTH "plugin_path"

// timer
#define CFG_IT_CFG_UPD_INTVL "config_update_interval"
#define CFG_IT_PCY_UPD_INTVL "policy_update_interval"


#endif
