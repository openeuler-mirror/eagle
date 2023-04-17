/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022 All rights reserved.
 * PowerAPI licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: jiangyi
 * Create: 2023-04-15
 * Description: Support 7 frequency setting patterns.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "include/dvfs.h"
#include "include/dvfs_log.h"


#define MIN_FREQ  1000000
#define MAX_FREQ  2600000
#define FREQ_STEP 50000
#define MS_TO_US  1 // 微秒到毫秒的单位转换

/*
 * 功能描述: 将频率设置为最低频, 并检查频率设置是否成功
 */
static int freq_low(unsigned int policy_id, int interval)
{
    int ret = RET_OK;
    ret = write_with_check(policy_id, MIN_FREQ);
    usleep(10000); // 10000: 休眠10ms
    usleep(interval * MS_TO_US);
    if (ret != RET_OK) {
        dvfs_err("policy_id:%u, failed\n", policy_id);
    }

    return ret;
}

/*
 * 功能描述: 将频率设置为最高频, 并检查频率设置是否成功
 */
static int freq_high(unsigned int policy_id, int interval)
{
    int ret = RET_OK;
    ret = write_with_check(policy_id, MAX_FREQ);
    usleep(10000); // 10000: 休眠10ms
    usleep(interval * MS_TO_US);
    if (ret != RET_OK) {
        dvfs_err("policy_id:%u, failed\n", policy_id);
    }

    return ret;
}

static int freq_random(unsigned int policy_id, int interval)
{
    int ret = RET_OK;
    unsigned int length = (MAX_FREQ - MIN_FREQ) / FREQ_STEP + 1;
    unsigned int offset = MIN_FREQ / FREQ_STEP;
    unsigned int freq = (rand() % length + offset) * FREQ_STEP;
    ret = write_freq(policy_id, freq);
    usleep(interval * MS_TO_US);

    return ret;
}

static int stepup_stepdown(unsigned int policy_id, int interval)
{
    int ret = RET_OK;
    for (int i = MIN_FREQ; i <= MAX_FREQ; i += FREQ_STEP) {
        ret = write_with_check(policy_id, i);
        if (ret != RET_OK) {
            dvfs_err("policy_id:%u, failed\n", policy_id);
            return ret;
        }
        usleep(interval * MS_TO_US);
    }
    for (int i = MAX_FREQ; i >= MIN_FREQ; i -= FREQ_STEP) {
        ret = write_with_check(policy_id, i);
        if (ret != RET_OK) {
            dvfs_err("policy_id:%u, failed\n", policy_id);
            return ret;
        }
        usleep(interval * MS_TO_US);
    }
    return ret;
}

static int quickup_stepdown(unsigned int policy_id, int interval)
{
    int ret = RET_OK;

    ret = write_with_check(policy_id, MIN_FREQ);
    if (ret != RET_OK) {
        dvfs_err("policy_id:%u, failed\n", policy_id);
        return ret;
    }
    usleep(interval * MS_TO_US);
    ret = write_with_check(policy_id, MAX_FREQ);
    if (ret != RET_OK) {
        dvfs_err("policy_id:%u, failed\n", policy_id);
        return ret;
    }
    usleep(interval * MS_TO_US);

    for (int i = MAX_FREQ; i >= MIN_FREQ; i -= FREQ_STEP) {
        ret = write_with_check(policy_id, i);
        if (ret != RET_OK) {
            dvfs_err("policy_id:%u, failed\n", policy_id);
            return ret;
        }
        usleep(interval * MS_TO_US);
    }
    return ret;
}

static int stepup_quickdown(unsigned int policy_id, int interval)
{
    int ret = RET_OK;
    for (int i = MIN_FREQ; i <= MAX_FREQ; i += FREQ_STEP) {
        ret = write_with_check(policy_id, i);
        if (ret != RET_OK) {
            dvfs_err("policy_id:%u, failed\n", policy_id);
            return ret;
        }
        usleep(interval * MS_TO_US);
    }
    ret = write_with_check(policy_id, MIN_FREQ);
    if (ret != RET_OK) {
        dvfs_err("policy_id:%u, failed\n", policy_id);
        return ret;
    }
    usleep(interval * MS_TO_US);

    return ret;
}

static int quickup_quickdown(unsigned int policy_id, int interval)
{
    int ret = RET_OK;

    ret = write_with_check(policy_id, MAX_FREQ);
    if (ret != RET_OK) {
        dvfs_err("policy_id:%u, failed\n", policy_id);
        return ret;
    }
    usleep(interval * MS_TO_US);
    ret = write_with_check(policy_id, MIN_FREQ);
    if (ret != RET_OK) {
        dvfs_err("policy_id:%u, failed\n", policy_id);
        return ret;
    }
    usleep(interval * MS_TO_US);

    return ret;
}

struct func_map_t {
    int func_id;
    int (*func)(unsigned int policy_id, int interval);
};

struct func_map_t func_map[PATTERN_NUM] = {
    {0, freq_low},
    {1, freq_high},
    {2, freq_random},
    {3, stepup_stepdown},
    {4, quickup_stepdown},
    {5, stepup_quickdown},
    {6, quickup_quickdown},
};

int run_pattern_func(int func_id, unsigned int policy_id, int interval)
{
    int ret = RET_ERR;
    if (func_id < PATTERN_NUM) {
        ret = func_map[func_id].func(policy_id, interval);
        dvfs_debug("policy_id:%u, func_id:%u\n", policy_id, func_id);
    }
    if (ret != RET_OK) {
        dvfs_err("policy_id:%u, failed\n", policy_id);
    }

    return ret;
}
