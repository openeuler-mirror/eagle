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
#ifndef EAGLE_POLICY_DT_H__
#define EAGLE_POLICY_DT_H__

#include "public.h"

enum PcyState {
    PCY_DISABLE = 0,
    PCY_ENABLE = 1,
    PCY_NONE = 99
};

typedef struct PcyBase {
    int enable;
    char lib[MAX_KEY_LEN];
} PcyBase;

typedef struct SchedServicePcy {
    PcyBase base;
    int wattEnable;
    int wattThreshold;     // [0,100]
    int wattInterval;      // [0, 3600000] ms
    int wattMask;
    int wattFirstDomain;
    int sgEnable;       //  smart grid
    int sgGovEnable;
    char wattProcs[MAX_VALUE];
    char sgVipProcs[MAX_VALUE];
    char sgVipGov[MAX_KEY_LEN];
    char sgLev1Gov[MAX_KEY_LEN];
} SchedServicePcy;

typedef struct FreqServicePcy {
    PcyBase base;
    char freqGov[MAX_KEY_LEN];
    int perfLossRate;   // [-1,100]
    int samplingRate;   // us
} FreqServicePcy;

typedef struct IdleServicePcy {
    PcyBase base;
    char idleGov[MAX_KEY_LEN];
} IdleServicePcy;

typedef struct PcapServicePcy {
    PcyBase base;
    int enablePcap;
    int capTarget;
} PcapServicePcy;

typedef struct MpcServicePcy {
    PcyBase base;
    int enableMpc;
} MpcServicePcy;

typedef struct Policy {
    char pcyName[MAX_KEY_LEN];
    char pcyDesc[MAX_LINE_LENGTH];
    SchedServicePcy schedPcy;
    FreqServicePcy freqPcy;
    IdleServicePcy idlePcy;
    PcapServicePcy pcapPcy;
    MpcServicePcy mpcPcy;
} Policy;

#endif
