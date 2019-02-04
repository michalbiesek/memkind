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
#include "common.h"
#include "errno.h"

class MemkindDefaultKindTests: public :: testing::Test
{

protected:
    void SetUp()
    {}

    void TearDown()
    {}
};

TEST_F(MemkindDefaultKindTests, test_TC_MEMKIND_PmemMallocZero)
{
    void *test1 = nullptr;

    test1 = memkind_malloc(MEMKIND_DEFAULT, 0);
    ASSERT_TRUE(test1 == nullptr);
}

TEST_F(MemkindDefaultKindTests, test_TC_MEMKIND_PmemMallocSizeMax)
{
    void *test1 = nullptr;

    errno = 0;
    test1 = memkind_malloc(MEMKIND_DEFAULT, SIZE_MAX);
    ASSERT_TRUE(test1 == nullptr);
    ASSERT_TRUE(errno == ENOMEM);
}

TEST_F(MemkindDefaultKindTests, test_TC_MEMKIND_DefaultReallocZero)
{
    size_t size = 1 * KB;
    void *test = nullptr;
    void *new_test = nullptr;

    test = memkind_malloc(MEMKIND_DEFAULT, size);
    ASSERT_TRUE(test != nullptr);

    new_test = memkind_realloc(MEMKIND_DEFAULT, test, 0);
    ASSERT_TRUE(new_test == nullptr);
}

TEST_F(MemkindDefaultKindTests, test_TC_MEMKIND_DefaultReallocSizeMax)
{
    size_t size = 1 * KB;
    void *test = nullptr;
    void *new_test = nullptr;

    test = memkind_malloc(MEMKIND_DEFAULT, size);
    ASSERT_TRUE(test != nullptr);
    errno = 0;
    new_test = memkind_realloc(MEMKIND_DEFAULT, test, SIZE_MAX);
    ASSERT_TRUE(new_test == nullptr);
    ASSERT_TRUE(errno == ENOMEM);

    memkind_free(MEMKIND_DEFAULT, test);
}

TEST_F(MemkindDefaultKindTests,
       test_TC_MEMKIND_DefaultPosixMemalignWrongAlignmentLessThanVoidAndNotPowerOfTwo)
{
    void *test = nullptr;
    size_t size = 32;
    size_t wrong_alignment = 3;
    int ret;
    errno = 0;

    ret = memkind_posix_memalign(MEMKIND_DEFAULT, &test, wrong_alignment, size);
    ASSERT_TRUE(ret == EINVAL);
    ASSERT_TRUE(test == nullptr);
    ASSERT_TRUE(errno == 0);
}

TEST_F(MemkindDefaultKindTests,
       test_TC_MEMKIND_DefaultPosixMemalignWrongAlignmentLessThanVoidAndPowerOfTwo)
{
    void *test = nullptr;
    size_t size = 32;
    size_t wrong_alignment = sizeof(void *)/2;
    int ret;
    errno = 0;

    ret = memkind_posix_memalign(MEMKIND_DEFAULT, &test, wrong_alignment, size);
    ASSERT_TRUE(ret == EINVAL);
    ASSERT_TRUE(test == nullptr);
    ASSERT_TRUE(errno == 0);
}

TEST_F(MemkindDefaultKindTests,
       test_TC_MEMKIND_DefaultPosixMemalignWrongAlignmentNotPowerOfTwo)
{
    void *test = nullptr;
    size_t size = 32;
    size_t wrong_alignment = sizeof(void *)+1;
    int ret;
    errno = 0;

    ret = memkind_posix_memalign(MEMKIND_DEFAULT, &test, wrong_alignment, size);
    ASSERT_TRUE(ret == EINVAL);
    ASSERT_TRUE(test == nullptr);
    ASSERT_TRUE(errno == 0);
}

TEST_F(MemkindDefaultKindTests,
       test_TC_MEMKIND_DefaultPosixMemalignLowestCorrectAlignment)
{
    void *test = nullptr;
    size_t size = 32;
    size_t alignment = sizeof(void *);
    int ret;

    ret = memkind_posix_memalign(MEMKIND_DEFAULT, &test, alignment, size);
    ASSERT_TRUE(ret == 0);
    ASSERT_TRUE(test != nullptr);

    memkind_free(MEMKIND_DEFAULT, test);
}

TEST_F(MemkindDefaultKindTests, test_TC_MEMKIND_DefaultPosixMemalignSizeZero)
{
    void *test = nullptr;
    size_t alignment = sizeof(void *);
    int ret;
    errno = 0;

    ret = memkind_posix_memalign(MEMKIND_DEFAULT, &test, alignment, 0);
    ASSERT_TRUE(ret == ENOMEM);
    ASSERT_TRUE(test == nullptr);
    ASSERT_TRUE(errno == 0);
}

TEST_F(MemkindDefaultKindTests, test_TC_MEMKIND_DefaultPosixMemalignSizeMax)
{
    void *test = nullptr;
    size_t alignment = 64;
    int ret;
    errno = 0;

    ret = memkind_posix_memalign(MEMKIND_DEFAULT, &test, alignment, SIZE_MAX);
    ASSERT_TRUE(ret == ENOMEM);
    ASSERT_TRUE(test == nullptr);
    ASSERT_TRUE(errno == 0);
}
