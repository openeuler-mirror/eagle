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
 * Description: Write or read frequencies by accessing file nodes.
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "include/dvfs.h"
#include "include/dvfs_log.h"

static int g_cpu_num = 32;

int write_freq(unsigned int policy_id, unsigned int freq)
{
    char path[100] = {0}; // 100: 定义1个size为100的数组
    unsigned int id = policy_id * g_cpu_num;
    // 100: path的最大size为100
    snprintf(path, 100, "%s%u%s", "/sys/devices/system/cpu/cpufreq/policy", id, "/scaling_setspeed");

    char freq_buf[20] = {0};
    snprintf(freq_buf, 20, "%u", freq); // 20: freq_buf的最大size为20

    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        printf("open %s failed\n", path);
        return RET_ERR;
    }
    int numwrite = write(fd, freq_buf, strlen(freq_buf));
    if (numwrite < 1) {
        printf("write file failed\n");
        close(fd);
        return RET_ERR;
    }
    close(fd);

    dvfs_debug("policy_id:%u, set frequency %u\n", policy_id, freq);

    return RET_OK;
}

int read_freq(unsigned int policy_id, unsigned int *freq)
{
    char path[100] = {0}; // 100: 定义1个size为100的数组
    unsigned int id = policy_id * g_cpu_num;
    // 100: path的最大size为100
    snprintf(path, 100, "%s%u%s", "/sys/devices/system/cpu/cpufreq/policy", id, "/cpuinfo_cur_freq");

    char freq_buf[20] = {0};
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("open %s failed\n", path);
        return RET_ERR;
    }
    int numread = read(fd, freq_buf, 19);
    if (numread < 1) {
        printf("read %s failed\n", path);
        close(fd);
        return RET_ERR;
    }
    close(fd);

    int ret = sscanf(freq_buf, "%u", freq);
    if (ret != 1) {
        return RET_ERR;
    }
    
    return RET_OK;
}

/*
 * 功能描述: 写入频率, 并检查写入是否生效
 */
int write_with_check(unsigned int policy_id, unsigned int freq)
{
    int ret = RET_OK;
    unsigned int actual_freq = 0;

    ret = write_freq(policy_id, freq);
    if (ret != RET_OK) {
        return ret;
    }
    usleep(50); // 写入之后等待50us,再做检查
    ret = RET_ERR;
    // 5: 连续5次查询频率写入是否生效, 每次查询间隔1ms
    for (int i = 0; i < 5; i++) {
        read_freq(policy_id, &actual_freq);
        if (actual_freq == freq) {
            ret = RET_OK;
            break;
        }
        usleep(1000); // 1000: 每次查询间隔1ms
    }

    return ret;
}

void set_cpu_num(int cpu_num)
{
    g_cpu_num = cpu_num;
}
