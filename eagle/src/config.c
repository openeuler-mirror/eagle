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
 * Description: loading config file and manager all config items for eagle
 * **************************************************************************** */

#include "config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "log.h"
#include "utils.h"

static char g_configPath[MAX_PATH_NAME] = "/etc/sysconfig/eagle_config.ini";
static char g_lastMd5[MD5_LEN] = {0};

static struct LogCfg g_logCfg;
inline LogCfg *GetLogCfg(void)
{
    return (LogCfg *)&g_logCfg;
}

static struct PolicyCfg g_policyCfg;
inline PolicyCfg *GetPolicyCfg(void)
{
    return (PolicyCfg *)&g_policyCfg;
}

static struct TimerCfg g_timerCfg;
inline TimerCfg *GetTimerCfg(void)
{
    return (TimerCfg *)&g_timerCfg;
}

static struct CnfItem CnfItemMap[CNF_ITEM_CNT] = {
    {E_CFG_IT_FLS, CFG_IT_FLS},
    {E_CFG_IT_CNT, CFG_IT_CNT},
    {E_CFG_IT_LGV, CFG_IT_LGV},
    {E_CFG_IT_LGP, CFG_IT_LGP},
    {E_CFG_IT_BKP, CFG_IT_BKP},
    {E_CFG_IT_PFX, CFG_IT_PFX},
    {E_CFG_IT_PCY, CFG_IT_PCY},
    {E_CFG_IT_CPU, CFG_IT_CPU},
    {E_CFG_IT_DSK, CFG_IT_DSK},
    {E_CFG_IT_NET, CFG_IT_NET},
    {E_CFG_IT_CFG_UPD_INTVL, CFG_IT_CFG_UPD_INTVL},
    {E_CFG_IT_PCY_UPD_INTVL, CFG_IT_PCY_UPD_INTVL}
};

static int InitLogCfg(void)
{
    bzero(&g_logCfg, sizeof(g_logCfg));
    g_logCfg.logLevel = DEBUG; // todo 发布时修改为INFO
    g_logCfg.maxFileSize = DEFAULT_FILE_SIZE * UNIT_FACTOR - MAX_LINE_LENGTH;
    g_logCfg.maxCmpCnt = DEFAULT_FILE_NUM;
    strncpy(g_logCfg.logPath, DEFAULT_LOG_PATH, sizeof(g_logCfg.logPath) - 1);
    strncpy(g_logCfg.logBkp, DEFAULT_LOG_PATH_BAK, sizeof(g_logCfg.logBkp) - 1);
    strncpy(g_logCfg.logPfx, DEFAULT_LOG_PFX, sizeof(g_logCfg.logPfx) - 1);
    return SUCCESS;
}

static int InitPolicyCfg(void)
{
    bzero(&g_policyCfg, sizeof(g_policyCfg));
    strncpy(g_policyCfg.policyFile, DEFAULT_POLICY_FILE_PATH, sizeof(g_policyCfg.policyFile) - 1);
    strncpy(g_policyCfg.cpuPlugin, DEFAULT_CPU_PLUGIN, sizeof(g_policyCfg.cpuPlugin) - 1);
    strncpy(g_policyCfg.diskPlugin, DEFAULT_DISK_PLUGIN, sizeof(g_policyCfg.diskPlugin) - 1);
    strncpy(g_policyCfg.nicPlugin, DEFAULT_NIC_PLUGIN, sizeof(g_policyCfg.nicPlugin) - 1);
    return SUCCESS;
}

static int InitTimerCfg(void)
{
    g_timerCfg.cfgUpdataInterval = DEFAULT_CONFIG_UPDATE_INTERVAL;
    g_timerCfg.policyUpdateInterval = DEFAULT_POLICY_UPDATE_INTERVAL;
    return SUCCESS;
}

static int UpdatePolicyCfg(enum CnfItemType type, char *value)
{
    int ret;
    char realpathRes[MAX_FULL_NAME] = {0};
    switch (type) {
        case E_CFG_IT_PCY:
            ret = NormalizeAndVerifyFilepath(value, realpathRes);
            if (ret != SUCCESS) {
                Logger(ERROR, MD_NM_CFG, "Policy_file_path in config is invalid. ret:%d", ret);
                return ERR_INVALIDE_PARAM;
            }
            strncpy(g_policyCfg.policyFile, value, sizeof(g_policyCfg.policyFile) - 1);
            break;
        case E_CFG_IT_CPU:
            strncpy(g_policyCfg.cpuPlugin, value, sizeof(g_policyCfg.cpuPlugin) - 1);
            break;
        case E_CFG_IT_DSK:
            strncpy(g_policyCfg.diskPlugin, value, sizeof(g_policyCfg.diskPlugin) - 1);
            break;
        case E_CFG_IT_NET:
            strncpy(g_policyCfg.nicPlugin, value, sizeof(g_policyCfg.nicPlugin) - 1);
            break;
        default:
            break;
    }
    return SUCCESS;
}

static int UpdateTimerCfg(enum CnfItemType type, char *value)
{
    int actualValue;
    switch (type) {
        case E_CFG_IT_CFG_UPD_INTVL:
            actualValue = atoi(value);
            if (!IsNumStr(value) || actualValue <= 0 || actualValue > MAX_PERIOD) {
                Logger(ERROR, MD_NM_CFG, "Config_update_interval in config is invalid");
                return ERR_INVALIDE_PARAM;
            }
            g_timerCfg.cfgUpdataInterval = actualValue;
            break;
        case E_CFG_IT_PCY_UPD_INTVL:
            actualValue = atoi(value);
            if (!IsNumStr(value) || actualValue <= 0 || actualValue > MAX_PERIOD) {
                Logger(ERROR, MD_NM_CFG, "Policy_update_interval in config is invalid");
                return ERR_INVALIDE_PARAM;
            }
            g_timerCfg.policyUpdateInterval = actualValue;
            break;
        default:
            break;
    }
    return SUCCESS;
}

static int UpdateLogCfg(enum CnfItemType type, char *value)
{
    int ret;
    int actualValue;
    char realpathRes[MAX_FULL_NAME] = {0};
    switch (type) {
        case E_CFG_IT_FLS:
            actualValue = atoi(value);
            if (!IsNumStr(value) || actualValue <= 0 || actualValue > MAX_FILE_SIZE) {
                Logger(ERROR, MD_NM_CFG, "File_size in config is invalid");
                return ERR_INVALIDE_PARAM;
            }

            g_logCfg.maxFileSize = actualValue * UNIT_FACTOR - MAX_LINE_LENGTH;
            break;
        case E_CFG_IT_CNT:
            actualValue = atoi(value);
            if (!IsNumStr(value) || actualValue <= 0 || actualValue > MAX_CMP_CNT) {
                Logger(ERROR, MD_NM_CFG, "Cmp_cnt in config is invalid");
                return ERR_INVALIDE_PARAM;
            }

            g_logCfg.maxCmpCnt = actualValue;
            break;
        case E_CFG_IT_LGV:
            actualValue = atoi(value);
            if (!IsNumStr(value) || actualValue < 0) {
                Logger(ERROR, MD_NM_CFG, "Log_level in config is invalid");
                return ERR_INVALIDE_PARAM;
            }

            g_logCfg.logLevel = actualValue;
            break;
        case E_CFG_IT_LGP:
            ret = NormalizeAndVerifyFilepath(value, realpathRes);
            if (ret != SUCCESS) {
                Logger(ERROR, MD_NM_CFG, "Log_path in config is invalid. ret:%d", ret);
                return ERR_INVALIDE_PARAM;
            }
            strncpy(g_logCfg.logPath, value, sizeof(g_logCfg.logPath) - 1);
            break;
        case E_CFG_IT_BKP:
            ret = NormalizeAndVerifyFilepath(value, realpathRes);
            if (ret != SUCCESS) {
                Logger(ERROR, MD_NM_CFG, "Bak_log_path in config is invalid. ret:%d", ret);
                return ERR_INVALIDE_PARAM;
            }
            strncpy(g_logCfg.logBkp, value, sizeof(g_logCfg.logBkp) - 1);
            break;
        case E_CFG_IT_PFX:
            strncpy(g_logCfg.logPfx, value, sizeof(g_logCfg.logPfx) - 1);
            char strFlRgx[MAX_NAME] = {0};
            if (sprintf(strFlRgx, "^%s-[[:digit:]]{14}.tar.gz$", g_logCfg.logPfx) < 0) {
                return ERR_SYS_EXCEPTION;
            }
            break;
        default:
            break;
    }
    return SUCCESS;
}

static enum CnfItemType StringToEnum(char *str)
{
    for (int i = 0; i < CNF_ITEM_CNT; i++) {
        if (strcmp(str, CnfItemMap[i].itemName) == 0) {
            return CnfItemMap[i].itemType;
        }
    }
    return DEFAULT_CONFIG_ITEM_TYPE;
}

int UpdateConfig(char *key, char *value)
{
    enum CnfItemType type = StringToEnum(key);

    switch (type) {
        case E_CFG_IT_FLS:
        case E_CFG_IT_CNT:
        case E_CFG_IT_LGV:
        case E_CFG_IT_LGP:
        case E_CFG_IT_BKP:
        case E_CFG_IT_PFX:
            UpdateLogCfg(type, value);
            break;
        case E_CFG_IT_PCY:
        case E_CFG_IT_CPU:
        case E_CFG_IT_DSK:
        case E_CFG_IT_NET:
            UpdatePolicyCfg(type, value);
            break;
        case E_CFG_IT_CFG_UPD_INTVL:
        case E_CFG_IT_PCY_UPD_INTVL:
            UpdateTimerCfg(type, value);
            break;
        default:
            break;
    }

    return SUCCESS;
}

static int GetCfgItem(char *realpathRes)
{
    char line[MAX_LINE_NUM] = {0};
    FILE *fp = fopen(realpathRes, "r");
    if (fp == NULL) {
        return ERR_NULL_POINTER;
    }
    while (fgets(line, sizeof(line) - 1, fp) != NULL) {
        // Skip invalid lines such as empty lines、comment lines
        if (strlen(line) <= 1 || line[0] == '#' || line[0] == '[') {
            continue;
        }
        // Parse the current line content, extract (key, value)
        char key[MAX_KEY_LEN] = {0};
        char value[MAX_LINE_LENGTH] = {0};
        char *index = strchr(line, '=');
        if (index == NULL) {
            continue;
        }

        strncpy(key, line, index - line);
        strncpy(value, index + 1, sizeof(value));
        LRtrim(key);
        LRtrim(value);
        if (strlen(key) == 0 || strlen(value) == 0) {
            // Key or value is invalid
            continue;
        }
        UpdateConfig(key, value);
    }
    (void)fclose(fp);
    return SUCCESS;
}

static int LoadConfigFile(void)
{
    char realpathRes[MAX_FULL_NAME] = {0};

    int ret = NormalizeAndVerifyFilepath(g_configPath, realpathRes);
    if (ret != SUCCESS) {
        return ret;
    }
    ret = GetCfgItem(realpathRes);
    return ret;
}

int InitConfig(void)
{
    // Get initial md5 of config
    bzero(g_lastMd5, sizeof(g_lastMd5));
    GetMd5(g_configPath, g_lastMd5);
    if (strlen(g_lastMd5) == 0) {
        Logger(ERROR, MD_NM_CFG, "Get initial md5 of config failed");
        return FAILED;
    }

    int ret = SUCCESS;
    // 1. Init by default values
    ret = InitLogCfg();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_CFG, "Init log config failed. ret:%d", ret);
        return ret;
    }

    ret = InitPolicyCfg();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_CFG, "Init policy config failed. ret:%d", ret);
        return ret;
    }

    ret = InitTimerCfg();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_CFG, "Init timer config failed. ret:%d", ret);
        return ret;
    }

    // 2. load config file
    ret = LoadConfigFile();
    if (ret != SUCCESS) {
        Logger(ERROR, MD_NM_CFG, "config file error. ret:%d", ret);
        return ret;
    }
    return SUCCESS;
}

int CheckAndUpdateConfig(void)
{
    char curMd5[MD5_LEN] = {0};
    GetMd5(g_configPath, curMd5);
    if (strlen(curMd5) == 0 || strcmp(curMd5, g_lastMd5) == 0) {
        return SUCCESS;
    } else {
        strncpy(g_lastMd5, curMd5, sizeof(g_lastMd5));
    }

    char realpathRes[MAX_FULL_NAME] = {0};
    int ret = NormalizeAndVerifyFilepath(g_configPath, realpathRes);
    if (ret != SUCCESS) {
        return ret;
    }
    ret = GetCfgItem(realpathRes);
    return ret;
}

int GetLogLevel(void)
{
    return g_logCfg.logLevel;
}

static enum LogLevel CauLeve(int level)
{
    enum LogLevel lgLvl;
    switch (level) {
        case DEBUG:
            lgLvl = DEBUG;
            break;
        case INFO:
            lgLvl = INFO;
            break;
        case WARNING:
            lgLvl = WARNING;
            break;
        default:
            lgLvl = ERROR;
    }
    return lgLvl;
}
