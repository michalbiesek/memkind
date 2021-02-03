// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2021 Intel Corporation. */

#include <memkind.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const size_t SIZE = 512;

static char *kind_allocation(const struct memkind * kind, const char* mem_type)
{
    char *ptr = (char *)memkind_malloc(kind, SIZE);
    if (ptr == NULL) {
        fprintf(stderr, "Unable to allocate %s memory \n", mem_type);
        exit(-1);
    }
    memset(ptr, 0, SIZE);
}

int main(int argc, char **argv)
{
    char *default_str = NULL;
    char *hbw_str = NULL;

    fprint(stdout , "Begin Allocation to DRAM memory\n");

    default_str = kind_allocation(MEMKIND_DEFAULT, "default");

    fprint(stdout , "End Allocation to DRAM memory\n");

    fprint(stdout , "Begin Allocation to High bandwidth memory\n");

    hbw_str = kind_allocation(MEMKIND_HBW, "HBW");

    fprint(stdout , "End Allocation to High bandwidth memory\n");


    //free memory

    memkind_free(MEMKIND_HBW, hbw_str);

    return 0;
}
