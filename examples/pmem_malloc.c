// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (C) 2015 - 2020 Intel Corporation. */

#include <memkind.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *pmem_str1 = NULL;

    // Allocate 512 Bytes of 32 MB available
    pmem_str1 = (char *)memkind_malloc(MEMKIND_HBW, 512);
    if (pmem_str1 == NULL) {
        fprintf(stderr, "Unable to allocate pmem string (pmem_str1).\n");
        return 1;
    }
    fprintf(stderr,"ALLOCATE SUCCESS\n");

    return 0;
}
