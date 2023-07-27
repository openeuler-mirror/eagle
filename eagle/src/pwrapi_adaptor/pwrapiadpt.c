/* *****************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023 All rights reserved.
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

#include "pwrapiadpt.h"
#include "log.h"


int PwrapiSetLogCallback(void(LogCallback)(int level, const char *fmt, va_list vl))
{
    int ret = PWR_SetLogCallback(LogCallback);
    if (ret != PWR_SUCCESS) {
        Logger(ERROR, MD_NM_PWRAPI, "Invoke PWR_SetLogCallback failed. ret:%d", ret);
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiRegister(void)
{
    int ret = PWR_Register();
    Logger(ERROR, MD_NM_PWRAPI, "Invoke PWR_Register failed. ret:%d", ret);
    if (ret != PWR_SUCCESS) {
        return ERR_INVOKE_PWRAPI_FAILED;
    }
    return SUCCESS;
}

int PwrapiUnRegister(void)
{
    (void)PWR_UnRegister();
    return SUCCESS;
}
