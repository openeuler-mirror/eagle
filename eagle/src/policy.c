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
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "log.h"
#include "utils.h"

// Policy section name
#define PCY_SEC_PCY "policy"
#define PCY_SEC_CPU "cpu"
#define PCY_SEC_DISK "disk"
#define PCY_SEC_NET "net"
#define PCY_SEC_USB "usb"
// Policy item name
#define PCY_ITEM_PCY_NAME "name"
#define PCY_ITEM_PCY_DESC "desc"
#define PCY_ITEM_CPU_FREQGOV "freq_governor"
#define PCY_ITEM_CPU_IDLEGOV "idle_governor"
#define PCY_ITEM_CPU_LATCY "cpu_dma_latency"
// #define PCY_ITEM_CPU_TICKMODE "tick_mode"
#define PCY_ITEM_DISK_DYNTUN "disk_dynamic_tuning"
#define PCY_ITEM_DISK_ALPM "alpm"
#define PCY_ITEM_NET_DYNTUN "net_dynamic_tuning"
#define PCY_ITEM_USB_AUTOSPD "autosuspend"

#define MAX_CPU_DMA_LATENCY 2000000000
#define MIN_CPU_DMA_LATENCY (-1)
#define PCY_STATE_ENABLE "eable"
#define PCY_STATE_DISABLE "disable"

static const char g_freqGov[][MAX_KEY_LEN] = {
    {"seep"}, {"conservative"}, {"ondemand"}, {"userspace"}, {"powersave"}, {"performance"}
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

static int FullfillCpuItem(Policy *pcy, const char *name, const char *value)
{
    if (strcmp(name, PCY_ITEM_CPU_FREQGOV) == 0) {
        if (!IsValueInList(value, g_freqGov, sizeof(g_freqGov) / sizeof(g_freqGov[0]))) {
            return ERR_FILE_CONTENT_ERROR;
        }
        strncpy(pcy->cpuPolicy.freqGov, value, sizeof(pcy->cpuPolicy.freqGov) - 1);
        return SUCCESS;
    }
    if (strcmp(name, PCY_ITEM_CPU_IDLEGOV) == 0) {
        if (!IsValueInList(value, g_idleGov, sizeof(g_idleGov) / sizeof(g_idleGov[0]))) {
            return ERR_FILE_CONTENT_ERROR;
        }
        strncpy(pcy->cpuPolicy.idleGov, value, sizeof(pcy->cpuPolicy.idleGov) - 1);
        return SUCCESS;
    }
    if (strcmp(name, PCY_ITEM_CPU_LATCY) == 0) {
        if (!NumRangeChk(value, MIN_CPU_DMA_LATENCY, MAX_CPU_DMA_LATENCY)) {
            return ERR_FILE_CONTENT_ERROR;
        }
        sscanf(value, "%d", &pcy->cpuPolicy.cpuDmaLatency);
        return SUCCESS;
    }
    return ERR_FILE_CONTENT_ERROR;
}

static int FullfillDiskItem(Policy *pcy, const char *name, const char *value)
{
    if (strcmp(name, PCY_ITEM_DISK_DYNTUN) == 0) {
        if (strcmp(value, PCY_STATE_ENABLE) == 0) {
            pcy->diskPolicy.dynamicTuning = PCY_ENABLE;
        } else if (strcmp(value, PCY_STATE_DISABLE) == 0) {
            pcy->diskPolicy.dynamicTuning = PCY_DISABLE;
        } else {
            return ERR_FILE_CONTENT_ERROR;
        }
        return SUCCESS;
    }
    if (strcmp(name, PCY_ITEM_DISK_ALPM) == 0) {
        if (!IsValueInList(value, g_alpm, sizeof(g_alpm) / sizeof(g_alpm[0]))) {
            return ERR_FILE_CONTENT_ERROR;
        }
        strncpy(pcy->diskPolicy.alpm, value, sizeof(pcy->diskPolicy.alpm) - 1);
        return SUCCESS;
    }
    return ERR_FILE_CONTENT_ERROR;
}

static int FullfillNetItem(Policy *pcy, const char *name, const char *value)
{
    if (strcmp(name, PCY_ITEM_NET_DYNTUN) == 0) {
        if (strcmp(value, PCY_STATE_ENABLE) == 0) {
            pcy->netPolicy.dynamicTuning = PCY_ENABLE;
        } else if (strcmp(value, PCY_STATE_DISABLE) == 0) {
            pcy->netPolicy.dynamicTuning = PCY_DISABLE;
        } else {
            return ERR_FILE_CONTENT_ERROR;
        }
        return SUCCESS;
    }
    return ERR_FILE_CONTENT_ERROR;
}

static int FullfillUsbItem(Policy *pcy, const char *name, const char *value)
{
    if (strcmp(name, PCY_ITEM_USB_AUTOSPD) == 0) {
        if (strcmp(value, PCY_STATE_ENABLE) == 0) {
            pcy->usbPolicy.autoSuspend = PCY_ENABLE;
        } else if (strcmp(value, PCY_STATE_DISABLE) == 0) {
            pcy->usbPolicy.autoSuspend = PCY_DISABLE;
        } else {
            return ERR_FILE_CONTENT_ERROR;
        }
        return SUCCESS;
    }
    return ERR_FILE_CONTENT_ERROR;
}

static int FullfillPolicyItem(Policy *pcy, const char *section, const char *name, const char *value)
{
    if (strcmp(section, PCY_SEC_PCY) == 0) {
        return FullfillPcyItem(pcy, name, value);
    }
    if (strcmp(section, PCY_SEC_CPU) == 0) {
        return FullfillCpuItem(pcy, name, value);
    }
    if (strcmp(section, PCY_SEC_DISK) == 0) {
        return FullfillDiskItem(pcy, name, value);
    }
    if (strcmp(section, PCY_SEC_NET) == 0) {
        return FullfillNetItem(pcy, name, value);
    }
    if (strcmp(section, PCY_SEC_USB) == 0) {
        return FullfillUsbItem(pcy, name, value);
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
        end = FindChars(start, " = :");
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
    if(!pcy) {
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


