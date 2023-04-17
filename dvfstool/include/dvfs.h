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
 * Description:Header file for dvfs.
 */
#ifndef DVFS_H
#define DVFS_H

#define RET_ERR  (-1)
#define RET_OK   0

#define POLICY_NUM  4  // 4: DIE的数目为4, 或者说policy的数目为4
#define PATTERN_NUM 7  // 7: 支持的调频模式有7种

#ifdef __cplusplus
extern "C" {
#endif

int write_freq(unsigned int policy_id, unsigned int freq);
int read_freq(unsigned int policy_id, unsigned int *freq);
int run_multi_thread(int mode);
int write_with_check(unsigned int policy_id, unsigned int freq);
int run_pattern_func(int func_id, unsigned int policy_id, int interval);
void set_interval(int interval);
void set_cpu_num(int cpu_num);
void set_exit_flag(void);

#ifdef __cplusplus
}
#endif

#endif /* DVFS_H */
