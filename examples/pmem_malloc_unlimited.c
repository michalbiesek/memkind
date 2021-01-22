// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (C) 2018 - 2020 Intel Corporation. */

#include <memkind.h>

#include <sys/syscall.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Huge allocation
    char *ptr = (char *)memkind_malloc(MEMKIND_HBW, 32 * 1024 * 1024);
    if (ptr == NULL) {
        fprintf(stderr, "Allocation return NULL.\n");
    } else {
        fprintf(stderr, "Allocation success.\n");
    }

    memkind_free(MEMKIND_HBW, ptr);

    return 0;
}
