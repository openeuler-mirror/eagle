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
 * Description: Log function definition.
 */
#ifndef DVFS_LOG_H
#define DVFS_LOG_H

#include <stdio.h>

#define dvfs_err(fmt, args...) \
    do { \
        printf("[error][%s][%d]", __FUNCTION__, __LINE__); \
        printf(fmt, ##args); \
    } while (0)

#ifdef DVFS_DEBUG
#define dvfs_debug(fmt, args...) \
    do { \
        printf("[debug][%s][%d]", __FUNCTION__, __LINE__); \
        printf(fmt, ##args); \
    } while (0)
#else
#define dvfs_debug(fmt, args...)
#endif

#endif /* DVFS_LOG_H */
