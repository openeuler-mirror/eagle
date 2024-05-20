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

#include "servicemgr.h"
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include "common.h"
#include "policymgr.h"
#include "log.h"
#include "utils.h"

#define MAX_SERVICE_NUM 6

typedef int (*SrvFpSetLogCallback)(void(LogCallback)(int, const char *, const char *, va_list),
    const char *usrInfo);
typedef int (*SrvFpInit)(void);
typedef int (*SrvFpStart)(void* pcy);
typedef int (*SrvFpUpdate)(void* pcy);
typedef int (*SrvFpLooper)(void);
typedef int (*SrvFpStop)(int mode);
typedef int (*SrvFpUninit)(void);

enum ServiceState {
    ST_OFFLOAD = 0,     // lib and functions not loaded
    ST_LOADED = 1,      // lib and functions loaded and initiated
    ST_RUNNING = 2,     // service started.
};

typedef struct Service {
    int status;
    void *handle;
    SrvFpSetLogCallback setLogCallback;
    SrvFpInit           init;
    SrvFpStart          start;
    SrvFpUpdate         update;
    SrvFpLooper         looper;
    SrvFpStop           stop;
    SrvFpUninit         uninit;
} Service;

static struct Service services[MAX_SERVICE_NUM] = {0};

struct ServicePolicyMap {
    int idx;
    Service* service;
    PcyBase* curPcy;
    PcyBase* lastPcy;
};

static struct ServicePolicyMap srvPcyMap[MAX_SERVICE_NUM] = {0};
static void InitSrvPcyMap(void)
{ 
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        srvPcyMap[i].idx = i;
        srvPcyMap[i].service = &services[i];
    }
    srvPcyMap[0].curPcy = (PcyBase*)&(GetCurPolicy()->schedPcy);
    srvPcyMap[1].curPcy = (PcyBase*)&(GetCurPolicy()->freqPcy);
    srvPcyMap[2].curPcy = (PcyBase*)&(GetCurPolicy()->idlePcy);
    srvPcyMap[3].curPcy = (PcyBase*)&(GetCurPolicy()->pcapPcy);
    srvPcyMap[4].curPcy = (PcyBase*)&(GetCurPolicy()->mpcPcy);
    srvPcyMap[5].curPcy = NULL;
    srvPcyMap[0].lastPcy = (PcyBase*)&(GetLastPolicy()->schedPcy);
    srvPcyMap[1].lastPcy = (PcyBase*)&(GetLastPolicy()->freqPcy);
    srvPcyMap[2].lastPcy = (PcyBase*)&(GetLastPolicy()->idlePcy);
    srvPcyMap[3].lastPcy = (PcyBase*)&(GetLastPolicy()->pcapPcy);
    srvPcyMap[4].lastPcy = (PcyBase*)&(GetLastPolicy()->mpcPcy);
    srvPcyMap[5].lastPcy = NULL;
}

static void SrvLogCallback(int level, const char *usrInfo, const char *fmt, va_list vl)
{
    char message[MAX_LINE_NUM] = {0};
    if (vsnprintf(message, sizeof(message) - 1, fmt, vl) < 0) {
        return;
    }
    Logger(level, MD_NM_SVRMGR, "[%s] %s",usrInfo, message);
}

#define LoadFunction(func, funcType, symName)                                     \
    func = (funcType)dlsym(services[idx].handle, symName);                        \
    if (!func) {                                                                  \
        successful = FALSE;                                                       \
        Logger(ERROR, MD_NM_SVRMGR, "Load function failed. err:%s", dlerror());   \
    }

static inline void OffloadService(int idx)
{
    if (services[idx].handle) {
        dlclose(services[idx].handle);
    }
    // services[idx].status = ST_OFFLOAD;
    bzero(&services[idx], sizeof(services[idx]));
}

static int LoadService(int idx)
{
    if (services[idx].status == ST_LOADED) {
        return TRUE;
    }
    char file[MAX_FILE_NAME] = {0};
    GetFullFileName(file, GetPolicyCfg()->pluginPath, srvPcyMap[idx].curPcy->lib);
    services[idx].handle = dlopen(file, RTLD_LAZY | RTLD_GLOBAL);
    if (!services[idx].handle) {
        Logger(ERROR, MD_NM_SVRMGR, "dlopen failed. %s", dlerror());
        return ERR_DL_OPEN_FAILED;
    }
    int successful = TRUE;
    LoadFunction(services[idx].setLogCallback, SrvFpSetLogCallback, "SRV_SetLogCallback")
    LoadFunction(services[idx].init, SrvFpInit, "SRV_Init")
    LoadFunction(services[idx].start, SrvFpStart, "SRV_Start")
    LoadFunction(services[idx].update, SrvFpUpdate, "SRV_Update")
    LoadFunction(services[idx].looper, SrvFpLooper, "SRV_Looper")
    LoadFunction(services[idx].stop, SrvFpStop, "SRV_Stop")
    LoadFunction(services[idx].uninit, SrvFpUninit, "SRV_Uninit")
    if (!successful) {
        OffloadService(idx);
        return ERR_DL_OPEN_FAILED;
    }
    services[idx].setLogCallback(SrvLogCallback, srvPcyMap[idx].curPcy->lib);
    int ret = services[idx].init();
    if (ret != SUCCESS) {
        OffloadService(idx);
        return ret;
    }
    services[idx].status = ST_LOADED;
    return SUCCESS;
}

static inline void StopService(int idx, int mode)
{
    if (services[idx].stop) {
        services[idx].stop(mode);
        services[idx].status = ST_LOADED;
    }
}

static inline int StartService(int idx)
{
    if (!services[idx].start) {
        return ERR_NULL_POINTER;
    }
    int ret = services[idx].start(srvPcyMap[idx].curPcy);
    if (ret == SUCCESS) {
        services[idx].status = ST_RUNNING;
    }
    return ret;
}

static inline int ReloadAndStartService(int idx)
{
    (void)StopService(idx, EXIT_MODE_RESTORE);
    OffloadService(idx);
    int ret = LoadService(idx);
    if (ret != SUCCESS) {
        return ret;
    }
    return StartService(idx);
}

// public===========================================================================
int InitServiceMgr(void)
{
    InitSrvPcyMap();
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        if (!srvPcyMap[i].curPcy || srvPcyMap[i].curPcy->enable != PCY_ENABLE) {
            continue;
        }
        (void)LoadService(i);
    }
    return SUCCESS;
}

int StartServices(void)
{
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        if (services[i].status != ST_LOADED || services[i].start == NULL) {
            continue;
        }
        (void)StartService(i);
    }
    return SUCCESS;
}

void UpdateServices(void)
{
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        if (!srvPcyMap[i].curPcy) {
            continue;
        }

        if (srvPcyMap[i].curPcy->enable == PCY_DISABLE && services[i].status != ST_OFFLOAD) {
            // need offload the service
            if (services[i].status == ST_RUNNING) {
                StopService(i, EXIT_MODE_RESTORE);
            }
            OffloadService(i);
            continue;
        }

        if (srvPcyMap[i].curPcy->enable == PCY_ENABLE) {
            if (strcmp(srvPcyMap[i].curPcy->md5, srvPcyMap[i].lastPcy->md5) != 0) {
                // plugin lib file updated
                ReloadAndStartService(i);
                continue;
            }
            if (services[i].status == ST_OFFLOAD) {
                (void)LoadService(i);
                (void)StartService(i);
            }
            if (services[i].status == ST_LOADED) {
                (void)StartService(i);
            }
            continue;
        }

        if (services[i].status == ST_RUNNING && srvPcyMap[i].curPcy->modified) {
            services[i].update(srvPcyMap[i].curPcy);
        }
    }
}

void TriggerTimerForServices(void)
{
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        if (services[i].status == ST_RUNNING && services[i].looper) {
            services[i].looper();
        }
    }
}

void StopServices(int mode)
{
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        StopService(i, mode);
    }
}

void UninitServiceMgr(void)
{
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        if (services[i].status == ST_RUNNING && services[i].stop) {
            services[i].stop(EXIT_MODE_RESTORE);
        }
        if (services[i].uninit) {
            services[i].uninit();
        }
        OffloadService(i);
    }
}
