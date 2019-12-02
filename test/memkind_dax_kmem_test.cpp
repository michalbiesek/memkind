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

#include <cstring>
#include <climits>
#include <numa.h>
#include <numaif.h>
#include <unistd.h>
#include <vector>

#include "common.h"

class MemkindDaxKmemTests: public ::testing::Test
{

protected:
    void SetUp()
    {}

    void TearDown()
    {}
};

static size_t get_used_swap() {
    size_t VmSwap;
    bool status = false;
    FILE* fp = fopen("/proc/self/status", "r");
    if (fp) {
        char buffer[BUFSIZ];
        while(fgets(buffer, sizeof (buffer), fp)) {
            if (sscanf(buffer, "VmSwap: %zu kB", &VmSwap) == 1){
                status = true;
                break;
            }
        }
        fclose(fp);
    }
    assert(!status && "Couldn't access swap space");
    return VmSwap * KB;
}

TEST_F(MemkindDaxKmemTests,
       test_TC_MEMKIND_MEMKIND_DAX_KMEM_alloc_until_full_numa)
{
    size_t numa_size;
    int numa_id = -1;
    const size_t alloc_size = 100 * MB;
    std::vector<void *> allocations;
    void *ptr = memkind_malloc(MEMKIND_DAX_KMEM, alloc_size);
    ASSERT_NE(nullptr, ptr);
    memset(ptr, 'a', alloc_size);
    allocations.push_back(ptr);

    get_mempolicy(&numa_id, nullptr, 0, ptr, MPOL_F_NODE | MPOL_F_ADDR);
    numa_size = numa_node_size64(numa_id, nullptr);

    while (numa_size > alloc_size * allocations.size()) {
        ptr = memkind_malloc(MEMKIND_DAX_KMEM, alloc_size);
        memset(ptr, 'a', alloc_size);
        allocations.push_back(ptr);
    }

    for(int i = 0; i < 20; ++i) {
        ptr = memkind_malloc(MEMKIND_DAX_KMEM, alloc_size);
        ASSERT_NE(nullptr, ptr);
        memset(ptr, 'a', alloc_size);
    }
    size_t ddd = get_used_swap();
    (void)ddd;
//    ASSERT_TRUE(is_swap_used(alloc_size / KB));

    for (auto const &ptr: allocations) {
        memkind_free(MEMKIND_DAX_KMEM, ptr);
    }
}
