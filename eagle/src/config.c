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
#include "log.h"

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

static int LoadConfigFile(void)
{
    // todo 打开读取文件，解析配置项，校验配置项，有效配置项替换
}

int InitConfig(void)
{
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
    // todo 检查配置文件是否有更新，有更新则更新系统配置项
}

int UpdateLogLevel(enum LogLevel logLevel)
{
    g_logCfg.logLevel = logLevel;
    return SUCCESS;
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
