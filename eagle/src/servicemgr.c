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

#define MAX_SERVICE_NUM 6

typedef int (*SrvFpSetLogCallback)(void(LogCallback)(int, const char *, const char *, va_list),
    const char *usrInfo);
typedef int (*SrvFpInit)(void);
typedef int (*SrvFpStart)(void* pcy);
typedef int (*SrvFpUpdate)(void* pcy);
typedef int (*SrvFpStop)(void);
typedef int (*SrvFpUninit)(void);

enum ServiceState {
    ST_OFFLOAD = 0,
    ST_LOADED = 1,
    ST_RUNNING = 2,
};

typedef struct Service {
    int status;
    void *handle;
    SrvFpSetLogCallback setLogCallback;
    SrvFpInit           init;
    SrvFpStart          start;
    SrvFpUpdate         update;
    SrvFpStop           stop;
    SrvFpUninit         uninit;
} Service;

static struct Service services[MAX_SERVICE_NUM] = {0};

struct ServicePolicyMap {
    int idx;
    Service* service;
    PcyBase* subPcy;
};

static struct ServicePolicyMap srvPcyMap[MAX_SERVICE_NUM] = {0};
static void InitSrvPcyMap(void)
{ 
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        srvPcyMap[i].idx = i;
        srvPcyMap[i].service = &services[i];
    }
    srvPcyMap[0].subPcy = (PcyBase*)&(GetCurPolicy()->schedPcy);
    srvPcyMap[1].subPcy = (PcyBase*)&(GetCurPolicy()->freqPcy);
    srvPcyMap[2].subPcy = (PcyBase*)&(GetCurPolicy()->idlePcy);
    srvPcyMap[3].subPcy = (PcyBase*)&(GetCurPolicy()->pcapPcy);
    srvPcyMap[4].subPcy = (PcyBase*)&(GetCurPolicy()->mpcPcy);
    srvPcyMap[5].subPcy = NULL;
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

static int LoadService(int idx)
{
    services[idx].handle = dlopen(srvPcyMap[idx].subPcy->lib, RTLD_LAZY | RTLD_GLOBAL);
    if (!services[idx].handle) {
        Logger(ERROR, MD_NM_SVRMGR, "dlopen failed. %s", dlerror());
        return ERR_DL_OPEN_FAILED;
    }
    int successful = TRUE;
    LoadFunction(services[idx].setLogCallback, SrvFpSetLogCallback, "SRV_SetLogCallback")
    LoadFunction(services[idx].init, SrvFpInit, "SRV_Init")
    LoadFunction(services[idx].start, SrvFpStart, "SRV_Start")
    LoadFunction(services[idx].update, SrvFpUpdate, "SRV_Update")
    LoadFunction(services[idx].stop, SrvFpStop, "SRV_Stop")
    LoadFunction(services[idx].uninit, SrvFpUninit, "SRV_Uninit")
    if (!successful) {
        bzero(&services[idx], sizeof(services[idx]));
        return ERR_DL_OPEN_FAILED;
    }
    return SUCCESS;
}

static void OffloadService(int idx)
{   
    if (services[idx].handle) {
        dlclose(services[idx].handle);
    }
    bzero(&services[idx], sizeof(services[idx]));
}

int InitServiceMgr(void)
{
    InitSrvPcyMap();
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        if (!srvPcyMap[i].subPcy || srvPcyMap[i].subPcy->enable != PCY_ENABLE) {
            continue;
        }
        if (LoadService(i) != SUCCESS) {
            continue;
        }
        services[i].setLogCallback(SrvLogCallback, srvPcyMap[i].subPcy->lib);
        if (services[i].init() == SUCCESS) {
            services[i].status = ST_LOADED;
        }
    }
    return SUCCESS;
}

int StartServices(void)
{
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        if (services[i].status != ST_LOADED || services[i].start == NULL) {
            continue;
        }
        if (services[i].start(srvPcyMap[i].subPcy) == SUCCESS) {
            services[i].status = ST_RUNNING;
        }
    }
    return SUCCESS;
}

void StopServices(void)
{
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        if (services[i].stop) {
            services[i].stop();
            services[i].status = ST_LOADED;
        }
    }
}

void UninitServiceMgr(void)
{
    for (int i = 0; i < MAX_SERVICE_NUM; i++) {
        if (services[i].status == ST_RUNNING && services[i].stop) {
            services[i].stop();
        }
        if (services[i].uninit) {
            services[i].uninit();
        }
        OffloadService(i);
    }
}
