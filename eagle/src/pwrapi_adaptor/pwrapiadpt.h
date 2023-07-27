/* *****************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * eagle licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: queyanwen
 * Create: 2023-06-28
 * Description: adaptor the powerapi
 * **************************************************************************** */
#ifndef EAGLE_PWRAPI_ADAPTOR_H__
#define EAGLE_PWRAPI_ADAPTOR_H__

#include <stdio.h>
#include "powerapi.h"

#ifdef __cplusplus
extern "C" {
#endif

int PwrapiSetLogCallback(void(LogCallback)(int level, const char *fmt, va_list vl));
int PwrapiRegister(void);
int PwrapiUnRegister(void);

#ifdef __cplusplus
}
#endif

#endif
