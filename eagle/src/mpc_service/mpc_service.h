/* *****************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 * eagle licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: heppen
 * Create: 2024-05-20
 * Description: provide intelligent mpc service.
 * **************************************************************************** */
#ifndef EAGLE_MPC_SERVICE_H__
#define EAGLE_MPC_SERVICE_H__

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SRV_API

SRV_API int SRV_SetLogCallback(void(LogCallback)(int level, const char *, const char *, va_list),
    const char *usrInfo);
SRV_API int SRV_Init(void);
SRV_API int SRV_Start(void* pcy);
SRV_API int SRV_Update(void* pcy);
SRV_API int SRV_Looper(void);
SRV_API int SRV_Stop(void);
SRV_API int SRV_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif // EAGLE_MPC_SERVICE_H__
