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
 * Description: Policy data structure
 * **************************************************************************** */
#ifndef EAGLE_POLICY_H__
#define EAGLE_POLICY_H__

#include "common.h"

enum PcyState {
    PCY_ENABLE = 1,
    PCY_DISABLE = 2
};

typedef struct CpuPolicy {
    char freqGov[MAX_KEY_LEN];
    char idleGov[MAX_KEY_LEN];
    char tickMode[MAX_KEY_LEN];
    int cpuDmaLatency;
} CpuPolicy;

typedef struct DiskPolicy {
    int dynamicTuning;
    char alpm[MAX_KEY_LEN];
} DiskPolicy;

typedef struct NetPolicy {
    int dynamicTuning;
} NetPolicy;

typedef struct UsbPolicy {
    int autoSuspend;
} UsbPolicy;

typedef struct Policy {
    char pcyName[MAX_KEY_LEN];
    char pcyDesc[MAX_VALUE];
    CpuPolicy cpuPolicy;
    DiskPolicy diskPolicy;
    NetPolicy netPolicy;
    UsbPolicy usbPolicy;
} Policy;

int InitPolicy(const char policyFilePath[], Policy *pcy);
Policy *GetCurPolicy(void);
Policy *GetLastPolicy(void);
#endif
