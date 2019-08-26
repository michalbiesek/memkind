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

#include <memkind.h>
#include <memkind/internal/memkind_arena.h>
#include "common.h"

#define PMEM_MAX_SIZE (1024 * 1024 * 64)
extern const char *PMEM_DIR;

class MemkindDefaultGetDefragHintTests : public ::testing::Test
{
protected:
    memkind_t pmem_kind;
    void SetUp()
    {
        // create PMEM partition
        int err = memkind_create_pmem(PMEM_DIR, PMEM_MAX_SIZE, &pmem_kind);
        ASSERT_EQ(0, err);
        ASSERT_NE(pmem_kind, nullptr);
    }

    void TearDown()
    {
        int err = memkind_destroy_kind(pmem_kind);
        ASSERT_EQ(0, err);
    }

};

/*
 * Determines if the pointer should be
 * reallocated in order to defragment the memory.
 */
//int StartDefrag(int bin, int run)
//{
//    /*
//     * If run utilisation is less than average utilisation in bin
//     * and if run is not full reallocate pointer.
//     */
//    if (run > bin || run == 1 << 16)
//        return 0;
//    else
//        return 1;
//}

TEST_F(MemkindDefaultGetDefragHintTests,
       test_TC_MEMKIND_PmemGetDefragHintReturnFalse)
{
//    int bin = -1;
//    int run = -1;
//    int hint = -1;
    const int alloc = 50000;
    void *ptr[alloc];

    for (int i = 0; i < alloc; i++) {
        ptr[i] = memkind_malloc(pmem_kind, 128);
        ASSERT_NE(ptr[i], nullptr);
    }

//    hint = memkind_check_defrag(ptr[alloc-1]);
//    ASSERT_EQ(hint, 0);

    for (int i = 0; i < alloc; i++) {
        memkind_free(pmem_kind, ptr[i]);
    }

    ptr[0] = memkind_malloc(pmem_kind, 50 * KB);
    ASSERT_NE(ptr[0], nullptr);
//    hint = memkind_check_defrag(ptr[0]);
//    ASSERT_EQ(hint, 0);
    memkind_free(pmem_kind, ptr[0]);
}

TEST_F(MemkindDefaultGetDefragHintTests,
       test_TC_MEMKIND_PmemGetDefragHintReturnTrue)
{
//    int bin = -1;
//    int run = -1;
//    int hint = -1;
    const int alloc = 50000;
    void *ptr[alloc];

    for (int i = 0; i < alloc; i++) {
        ptr[i] = memkind_malloc(pmem_kind, 128);
        ASSERT_NE(ptr[i], nullptr);
    }

//    hint = memkind_check_defrag(ptr[alloc / 2]);
//    ASSERT_EQ(hint, 1);

    for (int i = 0; i < alloc; i++) {
        memkind_free(pmem_kind, ptr[i]);
    }
}

TEST_F(MemkindDefaultGetDefragHintTests,
       test_TC_MEMKIND_PmemGetDefragHintCheckBinUtilizationReturnFalse)
{
//    int bin = -1;
//    int run = -1;
//    int hint = -1;
    const int alloc = 50000;
    void *ptr[alloc];
//    int startDefrag;

    for (int i = 0; i < alloc; i++) {
        ptr[i] = memkind_malloc(pmem_kind, 1 * KB);
        ASSERT_NE(ptr[i], nullptr);
    }

//    hint = memkind_check_defrag(ptr[alloc / 2]);
//    ASSERT_EQ(hint, 1);
//    startDefrag = memkind_check_defrag(ptr[alloc / 2]);
//    ASSERT_EQ(startDefrag, 0);

    for (int i = 0; i < alloc; i++) {
        memkind_free(pmem_kind, ptr[i]);
    }
}

TEST_F(MemkindDefaultGetDefragHintTests,
       test_TC_MEMKIND_PmemGetDefragHintCheckBinUtilizationReturnTrue)
{
//    int bin = -1;
//    int run = -1;
    int hint = -1;
    const int alloc = 50000;
    void *ptr[alloc];
    int startDefrag;

    for (int i = 0; i < alloc; i++) {
        ptr[i] = memkind_malloc(pmem_kind, 1 * KB);
        ASSERT_NE(ptr[i], nullptr);
    }

    for (int i = 1; i < alloc;) {
        memkind_free(pmem_kind, ptr[i]);
        ptr[i] = nullptr;
        // Free memory with irregular pattern
        if (i % 2 == 0)
            i += 3;
        else
            i += 5;
    }

    hint = memkind_check_defrag(ptr[10]);
    ASSERT_EQ(hint, MEMKIND_ERROR_OPERATION_FAILED);
    startDefrag = memkind_check_defrag(ptr[10]);
    ASSERT_EQ(startDefrag, MEMKIND_ERROR_OPERATION_FAILED);

    for (int i = 0; i < alloc; i++) {
        memkind_free(pmem_kind, ptr[i]);
    }
}
