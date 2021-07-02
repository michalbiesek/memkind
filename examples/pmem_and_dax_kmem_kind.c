// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (C) 2019 - 2020 Intel Corporation. */

#include <memkind.h>

#include <numa.h>
#include <numaif.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MB ((size_t)1 << 20)
#define GB ((size_t)1 << 30)
#define PAGE_SIZE (4096)
#define NODES_NO 4

int main(int argc, char *argv[])
{
    char *ptr_dax_kmem = NULL;

    ptr_dax_kmem = (char *)memkind_malloc(MEMKIND_DAX_KMEM_BALANCED, 8 * GB);
    if (!ptr_dax_kmem) {
        fprintf(stderr, "Unable allocate 512 bytes in balanced local memory NUMA node.\n");
        return 1;
    }
    size_t counter[NODES_NO] = {0};
    memset(ptr_dax_kmem, 0, 8*GB);
    int i;
    volatile char *cur_addr;
    char *addr_end;
    int target_node = -1;
    int status = -1;

    cur_addr = ptr_dax_kmem;
    addr_end = (char *)cur_addr + 8*GB;

    for (; cur_addr < addr_end; cur_addr += PAGE_SIZE) {
        status = get_mempolicy(&target_node, NULL, 0, (void*)cur_addr, MPOL_F_NODE | MPOL_F_ADDR);
        if (status) {
            return -1;
        }
        counter[target_node]++;
    }
    for (i=0; i<NODES_NO; i ++) {
        fprintf(stderr, "\nCounter id %d value %zu", i, counter[i]);
    }

    for (i=0; i<NODES_NO; i ++) {
        counter[i]=0;
    }

    cur_addr = ptr_dax_kmem;
    addr_end = (char *)cur_addr + 1*GB;
    for (i=0; i<10000; i ++) {
        for (; cur_addr < addr_end; cur_addr += PAGE_SIZE) {
                *cur_addr = *cur_addr;
        }
    }

    cur_addr = ptr_dax_kmem + 3*GB;
    addr_end = (char *)cur_addr + 1*GB;
    for (i=0; i<10000; i ++) {
        for (; cur_addr < addr_end; cur_addr += PAGE_SIZE) {
                *cur_addr = *cur_addr;
        }
    }

    cur_addr = ptr_dax_kmem;
    addr_end = (char *)cur_addr + 8*GB;
    for (; cur_addr < addr_end; cur_addr += PAGE_SIZE) {
        status = get_mempolicy(&target_node, NULL, 0, (void*)cur_addr, MPOL_F_NODE | MPOL_F_ADDR);
        if (status) {
            return -1;
        }
        counter[target_node]++;
    }

    for (i=0; i<NODES_NO; i ++) {
        fprintf(stderr, "\nCounter id %d value %zu", i, counter[i]);
    }

    memkind_free(MEMKIND_DAX_KMEM_BALANCED, ptr_dax_kmem);

    fprintf(stdout, "Memory was successfully allocated and released.\n");

    return 0;
}
