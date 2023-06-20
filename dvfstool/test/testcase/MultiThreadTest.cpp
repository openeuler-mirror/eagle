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
 * Description: testcases for multi_thread.c
 */
#include <cstdio>
#include <thread>
#include <gtest/gtest.h>
#include <dvfs.h>
using namespace std;

class MultiThreadTest : public ::testing::Test {
    protected:
        // 第一个用例执行前被调用
        static void SetUpTestCase()
        {
            set_cpu_num(sysconf(_SC_NPROCESSORS_CONF) / POLICY_NUM);
        }
        // 最后一个用例执行完被调用
        static void TearDownTestCase() {}

        // 每个用例执行前调用
        virtual void SetUp()
        {
            system("cpupower frequency-set -g userspace > /dev/null");
        }
        // 每个用例执行完被调用
        virtual void TearDown()
        {
            system("cpupower frequency-set -g performance > /dev/null");
        }
};

static void CallMultiThread(int mode)
{
    EXPECT_EQ(0, run_multi_thread(mode));
}

TEST_F(MultiThreadTest, run_multi_thread_001)
{
    thread t(CallMultiThread, 0);

    sleep(5); // 5: 休眠5s等待multi_thread执行一段时间
    set_exit_flag();
    t.join();
}

TEST_F(MultiThreadTest, run_multi_thread_002)
{
    set_exit_flag();
    EXPECT_EQ(0, run_multi_thread(1));
}

TEST_F(MultiThreadTest, run_multi_thread_003)
{
    set_exit_flag();
    EXPECT_EQ(0, run_multi_thread(2));
}
