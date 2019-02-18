/*
 * Copyright (C) 2019 Intel Corporation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice(s),
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice(s),
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
 * EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "memkind.h"

#include "allocator_perf_tool/TimerSysTime.hpp"
#include "common.h"

class MemkindNullKindTests: public :: testing::Test
{

protected:
    void SetUp()
    {}

    void TearDown()
    {}
};


TEST_F(MemkindNullKindTests, test_TC_MEMKIND_DefaultKindFreeNullPtr)
{
    const double test_time = 5;

    TimerSysTime timer;
    timer.start();
    do {
        memkind_free(MEMKIND_DEFAULT, nullptr);
    } while (timer.getElapsedTime() < test_time);
}

TEST_F(MemkindNullKindTests, test_TC_MEMKIND_NullKindFreeNullPtr)
{
    const double test_time = 5;

    TimerSysTime timer;
    timer.start();
    do {
        memkind_free(nullptr, nullptr);
    } while (timer.getElapsedTime() < test_time);
}

TEST_F(MemkindNullKindTests, test_TC_MEMKIND_DefaultRegularKindFreeNullPtr)
{
    const size_t size_1 = 1 * KB;
    const size_t size_2 = 1 * MB;
    void *ptr_default = nullptr;
    void *ptr_regular = nullptr;
    for (unsigned int i = 0; i < MEMKIND_MAX_KIND; ++i) {
        ptr_default = memkind_malloc(MEMKIND_DEFAULT, size_2);
        ASSERT_TRUE(nullptr != ptr_default);
        memkind_free(nullptr, ptr_default);
        ptr_default = memkind_malloc(MEMKIND_DEFAULT, size_1);
        ASSERT_TRUE(nullptr != ptr_default);
        memkind_free(nullptr, ptr_default);
        ptr_regular = memkind_malloc(MEMKIND_REGULAR, size_2);
        ASSERT_TRUE(nullptr != ptr_regular);
        memkind_free(nullptr, ptr_regular);
        ptr_regular = memkind_malloc(MEMKIND_REGULAR, size_1);
        ASSERT_TRUE(nullptr != ptr_regular);
        memkind_free(nullptr, ptr_regular);
    }
}
