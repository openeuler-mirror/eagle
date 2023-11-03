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
 * Description: loading policy file and manager all policy items.
 * **************************************************************************** */

#include "policy.h"
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "log.h"
#include "utils.h"

// Policy section name
#define PCY_SEC_PCY "policy"
#define PCY_SEC_SCHED "sched_service"
#define PCY_SEC_FREQ "freq_service"
#define PCY_SEC_IDLE "idle_service"
#define PCY_SEC_PCAP "pcap_service"
#define PCY_SEC_MPC  "mpc_service"
// Policy item name
#define PCY_ITEM_PCY_NAME "name"
#define PCY_ITEM_PCY_DESC "desc"
#define PCY_ITEM_BASE_EN   "enable"
#define PCY_ITEM_SCHED_LIB "sched_lib"
#define PCY_ITEM_SCHED_WATT_ENABLE "watt_sched_enable"
#define PCY_ITEM_SCHED_WATT_THRESHOLD "watt_threshold"
#define PCP_ITEM_SCHED_WATT_INTERVAL "watt_interval_ms"
#define PCP_ITEM_SCHED_WATT_DMSK "watt_domain_mask"
#define PCP_ITEM_SCHED_WATT_PROCS "watt_procs"
#define PCP_ITEM_SCHED_SG_ENABLE "smart_grid_enable"
#define PCP_ITEM_SCHED_SG_VIP_PROCS "smart_grid_vip_procs"
#define PCP_ITEM_SCHED_SG_GOV_ENABLE "smart_grid_gov_eable"
#define PCP_ITEM_SCHED_SG_VIP_GOV "smart_grid_vip_gov"
#define PCP_ITEM_SCHED_SG_LEV1_GOV "smart_grid_lev1_gov"

#define PCY_ITEM_FREQ_LIB "freq_lib"
#define PCY_ITEM_FREQ_GOV "cpufreq_gov"
#define PCY_ITEM_FREQ_PLR "perf_loss_rate"
#define PCY_ITEM_FREQ_SR "sampling_rate_us"

#define PCY_ITEM_IDLE_LIB "idle_lib"
#define PCY_ITEM_IDLE_GOV "cpuidle_gov"

#define PCY_ITEM_PCAP_LIB "pcap_lib"
#define PCY_ITEM_PCAP_ENPCAP "enable_pcap"
#define PCY_ITEM_PCAP_TGT "cap_target"

#define PCY_ITEM_MPC_LIB "mpc_lib"
#define PCY_ITEM_MPC_ENMPC "enable_mpc"

#define PCY_STATE_ENABLE "1"
#define PCY_STATE_DISABLE "0"

#define MAX_RATE 100
#define MIN_RATE 0
#define MIN_WATT_INTERVAL 0
#define MAX_WATT_INTERVAL 3600000   // ms
#define MAX_FREQ_SAM_RATE 10000000 // us
#define MIN_FREQ_SAM_RATE 0    // us
#define MAX_PCAP_TARGET 10000 // watt
#define MIN_PCAP_TARGET 100    // watt

static const char g_freqGov[][MAX_KEY_LEN] = {
    {"seep"}, {"conservative"}, {"ondemand"}, {"userspace"}, {"powersave"}, {"performance"},
    {"schedutil"}
};
static const char g_idleGov[][MAX_KEY_LEN] = {{"menu"}, {"teo"}};
static const char g_alpm[][MAX_KEY_LEN] = {{"min_power"}, {"medium_power"}, {"max_performance"}};


static int CheckFile(const char *file)
{
    if (file == NULL) {
        Logger(ERROR, MD_NM_PCY, "policyFilePath is NULL!");
        return ERR_NULL_POINTER;
    }

    if (access(file, F_OK | R_OK | W_OK) != 0) {
        Logger(ERROR, MD_NM_PCY, "File [%s] is not ok for read and write!", file);
        return ERR_FILE_NOT_EXIST;
    }
    return SUCCESS;
}

static char *FindFirstNonSpace(const char *s)
{
    const char *str = s;
    while (*str && isspace((unsigned char)(*str))) {
        str++;
    }
    return (char *)str;
}

static char *FindChars(const char *s, const char *chars)
{
    const char *str = s;
    while ((*str != '\0') && strchr(chars, *str) == NULL) {
        str++;
    }
    return (char *)str;
}

static int IsValueInList(const char *value, const char list[][MAX_KEY_LEN], const int len)
{
    if (!value || !list) {
        return FALSE;
    }
    for (int i = 0; i < len; i++) {
        if (strcmp(value, list[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

static inline int StrStateToIntState(const char *str, int *state)
{
    if (strcmp(str, PCY_STATE_ENABLE) == 0) {
        *state = PCY_ENABLE;
        return SUCCESS;
    }
    if (strcmp(str, PCY_STATE_DISABLE) == 0) {
        *state = PCY_DISABLE;
        return SUCCESS;
    }
    return ERR_FILE_CONTENT_ERROR;
}

static inline int StrToIntWithRangeChk(const char *str, int *rt, int min, int max)
{
    if (!NumRangeChk(str, min, max)) {
        return ERR_FILE_CONTENT_ERROR;
    }
    sscanf(str, "%d", rt);
    return SUCCESS;
}

static int FullfillPcyItem(Policy *pcy, const char *name, const char *value)
{
    if (strcmp(name, PCY_ITEM_PCY_NAME) == 0) {
        strncpy(pcy->pcyName, value, sizeof(pcy->pcyName) - 1);
        return SUCCESS;
    }
    if (strcmp(name, PCY_ITEM_PCY_DESC) == 0) {
        strncpy(pcy->pcyDesc, value, sizeof(pcy->pcyDesc) - 1);
        return SUCCESS;
    }
    return ERR_FILE_CONTENT_ERROR;
}

static int FullfillSchedItem(Policy *pcy, const char *name, const char *value)
{
    if (strcmp(name, PCY_ITEM_BASE_EN) == 0) {
        return StrStateToIntState(value, &pcy->schedPcy.base.enable);
    }
    if (strcmp(name, PCY_ITEM_SCHED_LIB) == 0) {
        strncpy(pcy->schedPcy.base.lib, value, sizeof(pcy->schedPcy.base.lib) - 1);
        return SUCCESS;
    }
    if (strcmp(name, PCY_ITEM_SCHED_WATT_ENABLE) == 0) {
        return StrStateToIntState(value, &pcy->schedPcy.wattEnable);
    }
    if (strcmp(name, PCY_ITEM_SCHED_WATT_THRESHOLD) == 0) {
        return StrToIntWithRangeChk(value, &pcy->schedPcy.wattThreshold, MIN_RATE, MAX_RATE);
    }
    if (strcmp(name, PCP_ITEM_SCHED_WATT_INTERVAL) == 0) {
        return StrToIntWithRangeChk(value, &pcy->schedPcy.wattInterval, MIN_WATT_INTERVAL, MAX_WATT_INTERVAL);
    }
    if (strcmp(name, PCP_ITEM_SCHED_WATT_DMSK) == 0) {
        return StrToIntWithRangeChk(value, &pcy->schedPcy.wattMask, 0, INT_MAX);
    }
    if (strcmp(name, PCP_ITEM_SCHED_WATT_PROCS) == 0) {
        strncpy(pcy->schedPcy.wattProcs, value, sizeof(pcy->schedPcy.wattProcs) - 1);
        return SUCCESS;
    }
    if (strcmp(name, PCP_ITEM_SCHED_SG_ENABLE) == 0) {
        return StrStateToIntState(value, &pcy->schedPcy.sgEnable);
    }
    if (strcmp(name, PCP_ITEM_SCHED_SG_VIP_PROCS) == 0) {
        strncpy(pcy->schedPcy.sgVipProcs, value, sizeof(pcy->schedPcy.sgVipProcs) - 1);
        return SUCCESS;
    }
    if (strcmp(name, PCP_ITEM_SCHED_SG_GOV_ENABLE) == 0) {
        return StrStateToIntState(value, &pcy->schedPcy.sgGovEnable);
    }
    if (strcmp(name, PCP_ITEM_SCHED_SG_VIP_GOV) == 0) {
        strncpy(pcy->schedPcy.sgVipGov, value, sizeof(pcy->schedPcy.sgVipGov));
        return SUCCESS;
    }
    if (strcmp(name, PCP_ITEM_SCHED_SG_LEV1_GOV) == 0) {
        strncpy(pcy->schedPcy.sgLev1Gov, value, sizeof(pcy->schedPcy.sgLev1Gov));
        return SUCCESS;
    }
    return ERR_FILE_CONTENT_ERROR;
}

static int FullfillFreqItem(Policy *pcy, const char *name, const char *value)
{
    if (strcmp(name, PCY_ITEM_BASE_EN) == 0) {
        return StrStateToIntState(value, &pcy->freqPcy.base.enable);
    }
    if (strcmp(name, PCY_ITEM_FREQ_LIB) == 0) {
        strncpy(pcy->freqPcy.base.lib, value, sizeof(pcy->freqPcy.base.lib) - 1);
        return SUCCESS;
    }
    if (strcmp(name, PCY_ITEM_FREQ_GOV) == 0) {
        if (!IsValueInList(value, g_freqGov, sizeof(g_freqGov) / sizeof(g_freqGov[0]))) {
            return ERR_FILE_CONTENT_ERROR;
        }
        strncpy(pcy->freqPcy.freqGov, value, sizeof(pcy->freqPcy.freqGov) - 1);
        return SUCCESS;
    }
    if (strcmp(name, PCY_ITEM_FREQ_PLR) == 0) {
        return StrToIntWithRangeChk(value, &pcy->freqPcy.perfLossRate, MIN_RATE - 1, MAX_RATE);
    }
    if (strcmp(name, PCY_ITEM_FREQ_SR) == 0) {
        return StrToIntWithRangeChk(value, &pcy->freqPcy.samplingRate, MIN_FREQ_SAM_RATE, MAX_FREQ_SAM_RATE);
    }
    return ERR_FILE_CONTENT_ERROR;
}

static int FullfillIdleItem(Policy *pcy, const char *name, const char *value)
{
    if (strcmp(name, PCY_ITEM_BASE_EN) == 0) {
        return StrStateToIntState(value, &pcy->idlePcy.base.enable);
    }
    if (strcmp(name, PCY_ITEM_IDLE_LIB) == 0) {
        strncpy(pcy->idlePcy.base.lib, value, sizeof(pcy->idlePcy.base.lib) - 1);
        return SUCCESS;
    }
    if (strcmp(name, PCY_ITEM_IDLE_GOV) == 0) {
        if (!IsValueInList(value, g_idleGov, sizeof(g_idleGov) / sizeof(g_idleGov[0]))) {
            return ERR_FILE_CONTENT_ERROR;
        }
        strncpy(pcy->idlePcy.idleGov, value, sizeof(pcy->idlePcy.idleGov) - 1);
        return SUCCESS;
    }
    return ERR_FILE_CONTENT_ERROR;
}

static int FullfillPcapItem(Policy *pcy, const char *name, const char *value)
{
    if (strcmp(name, PCY_ITEM_BASE_EN) == 0) {
        return StrStateToIntState(value, &pcy->pcapPcy.base.enable);
    }
    if (strcmp(name, PCY_ITEM_PCAP_LIB) == 0) {
        strncpy(pcy->pcapPcy.base.lib, value, sizeof(pcy->pcapPcy.base.lib) - 1);
        return SUCCESS;
    }
    if (strcmp(name, PCY_ITEM_PCAP_ENPCAP) == 0) {
        return StrStateToIntState(value, &pcy->pcapPcy.enablePcap);
    }
    if (strcmp(name, PCY_ITEM_PCAP_TGT) == 0) {
        return StrToIntWithRangeChk(value, &pcy->pcapPcy.capTarget, MIN_PCAP_TARGET, MAX_PCAP_TARGET);
    }
    return ERR_FILE_CONTENT_ERROR;
}

static int FullfillMpcItem(Policy *pcy, const char *name, const char *value)
{
    if (strcmp(name, PCY_ITEM_BASE_EN) == 0) {
        return StrStateToIntState(value, &pcy->mpcPcy.base.enable);
    }
    if (strcmp(name, PCY_ITEM_MPC_LIB) == 0) {
        strncpy(pcy->mpcPcy.base.lib, value, sizeof(pcy->mpcPcy.base.lib) - 1);
        return SUCCESS;
    }
    if (strcmp(name, PCY_ITEM_MPC_ENMPC) == 0) {
        return StrStateToIntState(value, &pcy->mpcPcy.enableMpc);
    }
    return ERR_FILE_CONTENT_ERROR;
}

static int FullfillPolicyItem(Policy *pcy, const char *section, const char *name, const char *value)
{
    // code to be optimized.
    if (strcmp(section, PCY_SEC_PCY) == 0) {
        return FullfillPcyItem(pcy, name, value);
    }
    if (strcmp(section, PCY_SEC_SCHED) == 0) {
        return FullfillSchedItem(pcy, name, value);
    }
    if (strcmp(section, PCY_SEC_FREQ) == 0) {
        return FullfillFreqItem(pcy, name, value);
    }
    if (strcmp(section, PCY_SEC_IDLE) == 0) {
        return FullfillIdleItem(pcy, name, value);
    }
    if (strcmp(section, PCY_SEC_PCAP) == 0) {
        return FullfillPcapItem(pcy, name, value);
    }
    if (strcmp(section, PCY_SEC_MPC) == 0) {
        return FullfillMpcItem(pcy, name, value);
    }
    return ERR_FILE_CONTENT_ERROR;
}

static int ParsePolicyItems(FILE *file, Policy *pcy)
{
    char line[MAX_LINE_NUM] = {0};
    char *start = NULL;
    char *end = NULL;
    char section[MAX_SEC_NAME] = {0};
    char *name = NULL;
    char *value = NULL;
    int ret = SUCCESS;

    while (ret == SUCCESS && fgets(line, MAX_LINE_NUM, file) != NULL) {
        start = line;
        start = FindFirstNonSpace(Rtrim(start));
        if (!(*start) || strchr(COMMENT_PREFIXES, *start) != NULL) {
            continue;
        }
        if (*start == '[') {
            // Section line
            end = FindChars(start + 1, "]");
            if (*end == ']') {
                *end = '\0';
                strncpy(section, start + 1, sizeof(section) - 1);
                continue;
            }
            ret = ERR_FILE_CONTENT_ERROR;
            break;
        }
        // Not a comment or section line, must be a name[ = :]value pair
        end = FindChars(start, "=:");
        if (*end == '=' || *end == ':') {
            *end = '\0';
            name = Rtrim(start);
            value = end + 1;
            value = FindFirstNonSpace(value);
            Rtrim(value);
            // Valid name[ = :]value pair found, call handler
            ret = FullfillPolicyItem(pcy, section, name, value);
        } else {
            // No ' = ' or ':' found on name[ = :]value line
            Logger(ERROR, MD_NM_PCY, "Policy file content error!");
            ret = ERR_FILE_CONTENT_ERROR;
        }
    }
    return ret;
}

static int ParseFile(const char *filePath, Policy *pcy)
{
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        return ERR_SYS_EXCEPTION;
    }
    int ret = ParsePolicyItems(file, pcy);
    if (fclose(file) != 0) {
        Logger(ERROR, MD_NM_PCY, "Close file [%s] failed.", filePath);
    }
    return ret;
}

/* *********************************************public****************************** */
int InitPolicy(const char policyFilePath[], Policy *pcy)
{
    if (!pcy) {
        return ERR_NULL_POINTER;
    }
    int ret = CheckFile(policyFilePath);
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCY, "CheckFile failed. ret:%d", ret);
        return ret;
    }
    ret = ParseFile(policyFilePath, pcy);
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_PCY, "ParseFile failed. ret:%d", ret);
        bzero(pcy, sizeof(Policy));
        return ret;
    }
    return SUCCESS;
}
