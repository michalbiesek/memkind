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
#include "vector"
#include <stdio.h>

extern const char *PMEM_DIR;
#define PMEM_MAX_SIZE (1 * GB)

class MemkindTransferAllocationTests: public ::testing::Test
{
protected:
    void SetUp()
    {}

    void TearDown()
    {}
};

TEST_F(MemkindTransferAllocationTests, test_TC_MEMKIND_NULLTransfer)
{
    void *ptr = memkind_transfer_allocation(nullptr, nullptr);
    ASSERT_EQ(ptr, nullptr);
    ptr = memkind_transfer_allocation(MEMKIND_REGULAR, nullptr);
    ASSERT_EQ(ptr, nullptr);
}

TEST_F(MemkindTransferAllocationTests, test_TC_MEMKIND_TCACHE_test)
{
    void * ptr = memkind_malloc(MEMKIND_DEFAULT, 512);
    ASSERT_NE(ptr, nullptr);
    void* new_ptr = memkind_transfer_allocation(MEMKIND_DEFAULT, ptr);
    ASSERT_EQ(new_ptr, nullptr);
}

TEST_F(MemkindTransferAllocationTests, test_TC_MEMKIND_TCACHE_HugeClassTest)
{
    void * ptr = memkind_malloc(MEMKIND_REGULAR, 1 * MB);
    ASSERT_NE(ptr, nullptr);
    void* new_ptr = memkind_transfer_allocation(MEMKIND_REGULAR, ptr);
    ASSERT_EQ(new_ptr, nullptr);
}

TEST_F(MemkindTransferAllocationTests, test_TC_MEMKIND_TCACHE_HugeClassTest2)
{
    void * ptr = memkind_malloc(MEMKIND_DEFAULT, 4 *KB + 10);
    ASSERT_NE(ptr, nullptr);
    void* new_ptr = memkind_transfer_allocation(MEMKIND_DEFAULT, ptr);
    ASSERT_EQ(new_ptr, nullptr);
}

TEST_F(MemkindTransferAllocationTests, test_TC_MEMKIND_success)
{
    struct memkind *kind = nullptr;
    unsigned i;
    const unsigned alloc = 50000;
    void *ptr[alloc];
    void *nptr;
    int err = memkind_create_pmem(PMEM_DIR, PMEM_MAX_SIZE, &kind);
    ASSERT_EQ(err, 0);

    for (i = 0; i < alloc; ++i) {
        ptr[i] = memkind_malloc(kind, 1 * KB);
        sprintf((char*)ptr[i], "memkind_malloc MEMKIND_PMEM\n");
        ASSERT_NE(ptr[i], nullptr);
    }

    for (i = 1; i < alloc;) {
        memkind_free(kind, ptr[i]);
        ptr[i] = nullptr;
        // Free memory with irregular pattern
        if (i % 2 == 0)
            i += 3;
        else
            i += 5;
    }

    nptr = memkind_transfer_allocation(kind, ptr[10]);
    ASSERT_NE(nptr, nullptr);

    err = memkind_destroy_kind(kind);
    ASSERT_EQ(err, 0);
}
