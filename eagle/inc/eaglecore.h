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
 * Description: provide configuration service
 * **************************************************************************** */
#ifndef EAGLE_CORE_H__
#define EAGLE_CORE_H__

int InitEagleSystem(void);
int StartEagleSystem(void);
void StopEagleSystem(int mode);
void CheckAndUpdatePolicy(void);
void TriggerTimer(void);
int HasNotifiedAuthReleased(void);
int RequestCtrlAuthFromPapis(void);
void ReleaseCtrlAuthFromPapis(int mode);
int HasPapisCtrlAuth(void);
void StartAllServices(void);
#endif
