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
#include "utils.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "common.h"
static struct timeval GetCurTv(void)
{
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    return curTime;
}
const char *GetCurFmtTmStr(const char *fmt, char *strTime, int bufLen)
{
    char strStdTime[MAX_STD_TIME] = {0};
    struct timeval curTime;
    struct tm *tmp = NULL;
    struct tm tmpTm;

    if (fmt == NULL) {
        return NULL;
    }
    curTime = GetCurTv();
    tmp = localtime_r(&curTime.tv_sec, &tmpTm);
    if (strftime(strStdTime, sizeof(strStdTime), fmt, tmp) == 0) {
        return NULL;
    }
    if ((int)strlen(strStdTime) > bufLen - 1) {
        return NULL;
    }
    strcpy(strTime, strStdTime);
    return strTime;
}

// Check head file
const char *GetCurFullTime(char *fullTime, int bufLen)
{
    int res;
    char strTime[MAX_FULL_TIME] = {0};
    char strStdTime[MAX_STD_TIME] = {0};
    struct timeval curTime;
    struct tm *tmp = NULL;
    struct tm tmpTm;

    curTime = GetCurTv();

    tmp = localtime_r(&curTime.tv_sec, &tmpTm);
    if (strftime(strStdTime, sizeof(strStdTime), "%F %T", tmp) == 0) {
        return NULL;
    }
    res = snprintf(strTime, sizeof(strTime) - 1, "%s.%ld", strStdTime, curTime.tv_usec / MS_TO_SEC);
    if (res < 0) {
        return NULL;
    }
    if ((int)strlen(strTime) > bufLen - 1) {
        return NULL;
    }
    strcpy(fullTime, strTime);
    return fullTime;
}

int MkDirs(const char *sDirName)
{
    int i;
    int len;
    char DirName[MAX_PATH_NAME] = {0};

    strcpy(DirName, sDirName);
    i = strlen(DirName);
    len = i;

    if (DirName[len - 1] != PATH_SEP_CHAR) {
        strcat(DirName, PATH_SEP_STR);
    }
    len = strlen(DirName);
    for (i = 1; i < len; i++) {
        if (DirName[i] == PATH_SEP_CHAR) {
            DirName[i] = '\0';
            if (access(DirName, F_OK) == 0) {
                DirName[i] = PATH_SEP_CHAR;
                continue;
            }
            if (mkdir(DirName, CRT_DIR_MODE) == -1) {
                return FAILED;
            }
            DirName[i] = PATH_SEP_CHAR;
        }
    }
    return SUCCESS;
}
int IsNumStr(const char *pStr)
{
    int len;
    int idx = 0;
    if (pStr == NULL) {
        return 0;
    }
    len = strlen(pStr);
    if (len > 0 && pStr[0] == '-') {
        ++idx;
    }
    for (; idx < len; ++idx) {
        if (!isdigit(pStr[idx])) {
            return 0;
        }
    }
    return 1;
}
int NumRangeChk(const char *pStr, long min, long max)
{
    int tmp;
    tmp = 0;

    if (!IsNumStr(pStr)) {
        return 0;
    }
    if (sscanf(pStr, "%d", &tmp) < 0) {
        return 0;
    }
    if (tmp < min || tmp > max) {
        return 0;
    }
    return 1;
}
// Find the last nonspace position, return pointer
char *Rtrim(char *s)
{
    if (s == NULL) {
        return NULL;
    }
    char *p = s + strlen(s);
    while (p - s > 0 && isspace((unsigned char)(*--p))) {
        *p = '\0';
    }
    return s;
}

int GetMd5(const char *filename, char *md5)
{
    char md5Cmd[MAX_NAME_LEN];
    const char s1[] = "md5sum ";
    const char s2[] = " | awk '{print $1}'";
    snprintf(md5Cmd, sizeof(md5Cmd), "%s%s%s", s1, filename, s2);

    FILE *fp = popen(md5Cmd, "r");
    if (fp == NULL) {
        return ERR_NULL_POINTER;
    }
    char buf[MD5_LEN] = {0};
    if (fgets(buf, sizeof(buf), fp) == NULL) {
        pclose(fp);
        return ERR_COMMON;
    }
    strncpy(md5, buf, MD5_LEN);
    pclose(fp);
    return SUCCESS;
}

// Remove the front and the last spaces of str.
void LRtrim(char *str)
{
    size_t length = strlen(str);
    int head = 0;
    int tail = length - 1;
    while (isspace(str[head])) {
        head++;
    }
    while (isspace(str[tail])) {
        tail--;
    }
    int i;
    for (i = 0; i <= tail - head; i++) {
        str[i] = str[head + i];
    }
    str[i] = '\0';
}

int NormalizeAndVerifyFilepath(const char *filename, char *realpathRes, int pathLen)
{
    if (realpathRes == NULL || filename == NULL) {
        return ERR_PATH_NORMALIZE;
    }
    char *path = NULL;
    path = realpath(filename, NULL);
    if (path == NULL) {
        return ERR_NULL_POINTER;
    }
    // strncpy(realpathRes, path, strlen(path));
    snprintf(realpathRes, pathLen, "%s", path);
    // Verify file path
    if (access(realpathRes, F_OK) != 0) {
        free(path);
        return ERR_PATH_VERIFY;
    }
    free(path);
    return SUCCESS;
}

int GetFullFileName(char *file, const char *path, const char *name)
{
    if (!file || !path || !name) {
        return ERR_NULL_POINTER;
    }
    strncpy(file, path, MAX_FILE_NAME - 1);
    if (file[strlen(file) - 1] != PATH_SEP_CHAR) {
        strncat(file, PATH_SEP_STR, MAX_FILE_NAME - strlen(file) - 1);
    }
    strncat(file, name, MAX_FILE_NAME - strlen(file) - 1);
    return SUCCESS;
}