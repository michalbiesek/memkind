// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (C) 2015 - 2020 Intel Corporation. */

#include <memkind.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{

    char *mem_str = NULL;

    while(1) {

    // Allocate 512 Bytes of 32 MB available
    mem_str = (char *)memkind_malloc(MEMKIND_HIGHEST_CAPACITY_LOCAL, 512);
    if (mem_str == NULL) {
        fprintf(stderr, "Unable to allocate pmem string (mem_str).\n");
        return 1;
    }
    memset(mem_str, 0 , 512);

    }

    fprintf(stdout, "Memory was successfully allocated and released.\n");

    return 0;
}
