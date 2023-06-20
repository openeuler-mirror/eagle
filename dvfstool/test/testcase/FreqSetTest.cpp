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
 * Description: testcases for freq_set.c
 */
#include <cstdio>
#include <gtest/gtest.h>
#include <dvfs.h>
using namespace std;

class FreqSetTest : public ::testing::Test {
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

TEST_F(FreqSetTest, write_freq_001)
{
    EXPECT_EQ(0, write_freq(0, 1000000));
}

TEST_F(FreqSetTest, write_freq_002)
{
    for (int i = 0; i < 1000; i++) {
        unsigned int policyId = rand() % POLICY_NUM;
        unsigned int freq = (rand() % 33 + 20) * 50000;
        EXPECT_EQ(0, write_freq(policyId, freq));

        unsigned int actualFreq = 0;
        EXPECT_EQ(0, read_freq(policyId, &actualFreq));
        EXPECT_EQ(actualFreq, freq);
    }
}
