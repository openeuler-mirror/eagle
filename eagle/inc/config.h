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
 * Author: queyanwen
 * Create: 2022-06-23
 * Description: provide configuration service
 * **************************************************************************** */
#ifndef EAGLE_CONFIG_H__
#define EAGLE_CONFIG_H__
#include <stdint.h>
#include "common.h"

#define DEFAULT_LOG_PATH "/opt/os_data/log"
#define DEFAULT_LOG_PATH_BAK "/opt/os_data/log/bak"
#define DEFAULT_LOG_PFX "eagle.log"
#define DEFAULT_FILE_SIZE 10 // MB
#define DEFAULT_FILE_NUM 3
#define DEFAULT_CONFIG_UPDATE_INTERVAL 75
#define DEFAULT_POLICY_UPDATE_INTERVAL 5
#define DEFAULT_POLICY_FILE_PATH "/etc/eagle/eagle.policy"
#define DEFAULT_CPU_PLUGIN "libeaglecpu.so"
#define DEFAULT_DISK_PLUGIN "libealgedisk.so"
#define DEFAULT_NIC_PLUGIN "libeaglenic.so"

#define DEFAULT_CONFIG_ITEM_TYPE (-1)
#define CNF_ITEM_CNT 12
#define MAX_FILE_SIZE 20
#define MAX_CMP_CNT 10

// LogCfg

enum CnfItemType {
    E_CFG_IT_FLS,
    E_CFG_IT_CNT,
    E_CFG_IT_LGV,
    E_CFG_IT_LGP,
    E_CFG_IT_BKP,
    E_CFG_IT_PFX,
    E_CFG_IT_PCY,
    E_CFG_IT_CPU,
    E_CFG_IT_DSK,
    E_CFG_IT_NET,
    E_CFG_IT_CFG_UPD_INTVL,
    E_CFG_IT_PCY_UPD_INTVL
};

typedef struct CnfItem {
    enum CnfItemType itemType;
    char *itemName;
} CnfItem;

typedef struct LogCfg {
    uint64_t maxFileSize;
    uint64_t maxCmpCnt;
    enum LogLevel logLevel;
    char logPath[MAX_PATH_NAME];
    char logBkp[MAX_PATH_NAME];
    char logPfx[MAX_PATH_NAME];
} LogCfg;

typedef struct PolicyCfg {
    char policyFile[MAX_FULL_NAME];
    char cpuPlugin[MAX_FILE_NAME];
    char diskPlugin[MAX_FILE_NAME];
    char nicPlugin[MAX_FILE_NAME];
} PolicyCfg;

// ServCfg
typedef struct TimerCfg {
    int cfgUpdataInterval;
    int policyUpdateInterval;
} TimerCfg;

int InitConfig(void);
LogCfg *GetLogCfg(void);
PolicyCfg *GetPolicyCfg(void);
TimerCfg *GetTimerCfg(void);
int CheckAndUpdateConfig(void);
#endif
