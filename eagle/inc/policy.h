/* *****************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * eagle licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: queyanwen
 * Create: 2022-06-23
 * Description: Policy data structure
 * **************************************************************************** */
#ifndef EAGLE_POLICY_H__
#define EAGLE_POLICY_H__

#include "common.h"
#include "policydt.h"

int CreatePolicy(const char policyFilePath[], Policy *pcy);
int PluginLibModified(const Policy *pcy);
int UpdateModifiedFlag(const Policy *oldPcy, Policy *newPcy);
#endif
