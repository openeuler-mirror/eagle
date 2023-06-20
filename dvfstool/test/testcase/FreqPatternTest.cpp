/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * PowerAPI licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: Jiang Yi
 * Create: 2023-06-20
 * Description: testcases for freq_pattern.c
 */
#include <cstdio>
#include <gtest/gtest.h>
#include <dvfs.h>
using namespace std;

class FreqPatternTest : public ::testing::Test {
    protected:
        // 第一个用例执行前被调用
        static void SetUpTestCase()
        {
            system("cpupower frequency-set -g userspace > /dev/null");
            set_cpu_num(sysconf(_SC_NPROCESSORS_CONF) / POLICY_NUM);
        }
        // 最后一个用例执行完被调用
        static void TearDownTestCase()
        {
            system("cpupower frequency-set -g performance > /dev/null");
        }

        // 每个用例执行前调用
        virtual void SetUp() {}
        // 每个用例执行完被调用
        virtual void TearDown() {}
};

TEST_F(FreqPatternTest, run_pattern_func_001)
{
    EXPECT_EQ(0, run_pattern_func(0, 0, 0));
}

TEST_F(FreqPatternTest, run_pattern_func_002)
{
    EXPECT_EQ(0, run_pattern_func(1, 0, 0));
}

TEST_F(FreqPatternTest, run_pattern_func_003)
{
    EXPECT_EQ(0, run_pattern_func(2, 0, 0));
}

TEST_F(FreqPatternTest, run_pattern_func_004)
{
    EXPECT_EQ(0, run_pattern_func(3, 0, 0));
}

TEST_F(FreqPatternTest, run_pattern_func_005)
{
    EXPECT_EQ(0, run_pattern_func(4, 0, 0));
}

TEST_F(FreqPatternTest, run_pattern_func_006)
{
    EXPECT_EQ(0, run_pattern_func(5, 0, 0));
}

TEST_F(FreqPatternTest, run_pattern_func_007)
{
    EXPECT_EQ(0, run_pattern_func(6, 0, 0));
}
