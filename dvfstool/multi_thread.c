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
 * Description: Create 4 threads to set the frequencies on different DIEs simultaneously.
 */
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include "include/dvfs.h"
#include "include/dvfs_log.h"

static int g_exit_flag = 0;
static int g_interval = 0;

struct pattern_t {
    pthread_mutex_t *p_mutex;
    unsigned int policy_id;
};

static int g_pattern_id[POLICY_NUM] = {0};

void set_exit_flag(void)
{
    g_exit_flag = 1;
}

static void *thread_run_pattern(void *arg)
{
    struct pattern_t *pattern = (struct pattern_t *)arg;
    pthread_mutex_t *p_mutex = pattern->p_mutex;
    unsigned int policy_id = pattern->policy_id;
    int pattern_id = 0;
    int ret = RET_OK;
    
    while (1) {
        pthread_mutex_lock(p_mutex);
        pattern_id = g_pattern_id[policy_id];
        pthread_mutex_unlock(p_mutex);
        ret = run_pattern_func(pattern_id, policy_id, g_interval);
        if (ret == RET_ERR) {
            dvfs_err("run_pattern_func failed, pattern_id:%d, policy_id:%u\n", pattern_id, policy_id);
            break;
        }
    }

    return NULL;
}

/*
 * 功能描述: 根据不同的mode选择不同的模式
 * mode 0: 所有可能的调频模式全组合
 * mode 1: 四个DIE随机选择高频/低频
 * mode 2: TA配置为高频, TB配置为低频
 */
int run_multi_thread(int mode)
{
    struct pattern_t pattern_inst[POLICY_NUM];
    pthread_t tid[POLICY_NUM];
    pthread_mutex_t m_mutex[POLICY_NUM];

    for (int i = 0; i < POLICY_NUM; i++) {
        pthread_mutex_init(&m_mutex[i], NULL);
        pattern_inst[i].p_mutex = &m_mutex[i];
        pattern_inst[i].policy_id = i;
        pthread_create(&tid[i], NULL, thread_run_pattern, &pattern_inst[i]);
    }

    while (1) {
        if (mode == 0) {
            for (int i = 0; i < (int)pow(PATTERN_NUM, POLICY_NUM); i++) {
                int divider = 1;
                for (int j = 0; j < POLICY_NUM; j++) {
                    int pattern_id = (i / divider) % PATTERN_NUM;
                    divider *= PATTERN_NUM;
                    pthread_mutex_lock(pattern_inst[j].p_mutex);
                    g_pattern_id[j] = pattern_id;
                    pthread_mutex_unlock(pattern_inst[j].p_mutex);
                }
                sleep(1);
                if (g_exit_flag != 0) {
                    break;
                }
                dvfs_debug("case_index:%u\n", i);
            }
        } else if (mode == 1) {
            for (int i = 0; i < POLICY_NUM; i++) {
                pthread_mutex_lock(pattern_inst[i].p_mutex);
                g_pattern_id[i] = rand() % 2; // 2: g_pattern_id的取值为0和1两个中的随机值
                pthread_mutex_unlock(pattern_inst[i].p_mutex);
            }
            sleep(1);
        } else {
            int fix_pattern_id[POLICY_NUM] = {1, 0, 1, 0};
            for (int i = 0; i < POLICY_NUM; i++) {
                pthread_mutex_lock(pattern_inst[i].p_mutex);
                g_pattern_id[i] = fix_pattern_id[i];
                pthread_mutex_unlock(pattern_inst[i].p_mutex);
            }
            sleep(1);
        }
        if (g_exit_flag != 0) {
            break;
        }
    }

    for (int i = 0; i < POLICY_NUM; i++) {
        pthread_cancel(tid[i]);
        pthread_mutex_destroy(&m_mutex[i]);
    }

    system("cpupower frequency-set -g performance > /dev/null");
    printf("change governor to performance\n");

    return 0;
}

void set_interval(int interval)
{
    g_interval = interval;
}

