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
 * Description: provide intelligent scheduling service
 * **************************************************************************** */
#ifndef EAGLE_SCHED_SERVICE_H__
#define EAGLE_SCHED_SERVICE_H__

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SRV_API


/**
Init the service
**/
SRV_API int SRV_SetLogCallback(void(LogCallback)(int level, const char *fmt, va_list vl));
SRV_API int SRV_Init(void);


#ifdef __cplusplus
}
#endif

#endif
