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
 * Author: luocaimin
 * Create: 2022-03-29
 * Description: provide common methods
 * **************************************************************************** */
#ifndef EAGLE_UTILS_H__
#define EAGLE_UTILS_H__

/**
 * Return the current time string in the specified format
 */
const char *GetCurFmtTmStr(const char *fmt, char *strTime, int bufLen);

/**
 * GetCurFullTime - return current time as "%Y - %m - %d %H:%M:%S.mss"
 */
const char *GetCurFullTime(char *fullTime, int bufLen);

// Create a multi - level directory
int MkDirs(const char *sDirName);

#endif
