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
 * Description: Entry function for dvfs tool.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "include/dvfs.h"

static void signal_exit_handler(int sig)
{
    set_exit_flag();
}

static void setup_signal(void)
{
    // register signal handler
    // the shell command kill generates SIGTERM by default
    (void)signal(SIGTERM, signal_exit_handler);
    (void)signal(SIGINT, signal_exit_handler);
}

int main(int argc, char *argv[])
{
    int mode = 0;
    int interval = 0;
    int cpu_num = 0;

    setup_signal();
    if (argc >= 2) { // 2: 如果只要1个输入参数
        sscanf(argv[1], "%d", &mode);
    }
    if (argc >= 3) { // 3: 如果有2个输入参数
        sscanf(argv[2], "%d", &interval);
    }
    printf("mode = %d, interval = %d\n", mode, interval);
    set_interval(interval);

    cpu_num = sysconf(_SC_NPROCESSORS_CONF);
    // 96, 128: 该dvfs工具仅用于对于96核或是128核的服务器环境进行测试
    if ((cpu_num != 96) && (cpu_num != 128)) {
        printf("cpu_num = %d, wrong parameter\n", cpu_num);
        return 0;
    }
    cpu_num /= POLICY_NUM;
    printf("cpu_num is %d\n", cpu_num);
    set_cpu_num(cpu_num);

    system("cpupower frequency-set -g userspace > /dev/null");
    printf("change governor to userspace\n");

    run_multi_thread(mode);
    return 0;
}

