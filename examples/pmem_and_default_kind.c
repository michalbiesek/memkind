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
#define ALLOC_SIZE (16 * GB)
#define ITERATION 100000
#define NO_PAGES (ALLOC_SIZE/PAGE_SIZE)

int get_node_id(void *addr){
    int ret;
    void * ptr_to_check = addr;

    int status[1];
    status[0]=-1;
    ret=move_pages(0, 1, &ptr_to_check, NULL, status, 0);
    if (ret) {
        exit(EXIT_FAILURE);
    }
    return status[0];
}

int main(int argc, char *argv[])
{
    char *ptr_dax_kmem = (char *)memkind_malloc(MEMKIND_DAX_KMEM_BALANCED, ALLOC_SIZE);
    if (!ptr_dax_kmem) {
        fprintf(stderr, "Unable allocate 512 bytes in balanced local memory NUMA node.\n");
        return 1;
    }
    size_t counter[NODES_NO] = {0};
    memset(ptr_dax_kmem, 0, ALLOC_SIZE);
    int i;
    volatile char *cur_addr;
    char *addr_end;

    cur_addr = ptr_dax_kmem;
    addr_end = (char *)cur_addr + ALLOC_SIZE;

    for (; cur_addr < addr_end; cur_addr += PAGE_SIZE) {
        int target_node = get_node_id((void*)cur_addr);
        counter[target_node]++;
    }

    fprintf(stderr, "\n Total Pages in allocation %zu", NO_PAGES);
    for (i=0; i<NODES_NO; i ++) {
        fprintf(stderr, "\nCounter id %d value %zu", i, counter[i]);
        counter[i]=0;
    }

    //touch every 4th page
    cur_addr = ptr_dax_kmem;
    addr_end = (char *)cur_addr + ALLOC_SIZE;
    for (i=0; i<ITERATION; i ++) {
        for (; cur_addr < addr_end; cur_addr += 4*PAGE_SIZE) {
            *cur_addr = *cur_addr;
        }
    }

    cur_addr = ptr_dax_kmem;
    addr_end = (char *)cur_addr + ALLOC_SIZE;
    for (; cur_addr < addr_end; cur_addr += PAGE_SIZE) {
        int target_node = get_node_id((void*)cur_addr);
        counter[target_node]++;
    }

    for (i=0; i<NODES_NO; i ++) {
        fprintf(stderr, "\nCounter id %d value %zu", i, counter[i]);
    }

    memkind_free(MEMKIND_DAX_KMEM_BALANCED, ptr_dax_kmem);

    fprintf(stdout, "Memory was successfully allocated and released.\n");

    return 0;
}
