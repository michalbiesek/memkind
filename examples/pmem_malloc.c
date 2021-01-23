// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (C) 2015 - 2020 Intel Corporation. */

#include <memkind.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *ptr = NULL;

    // Allocate 512 Bytes of 32 MB available
    ptr = (char *)memkind_malloc(MEMKIND_HIGHEST_BANDWIDTH_LOCAL, 512);
    if (ptr == NULL) {
        fprintf(stderr, "Unable to allocate local highest bandwdith (ptr).\n");
        return 1;
    }
    fprintf(stderr, "Success to allocate local highest bandwdith (ptr).\n");

    memkind_free(MEMKIND_HIGHEST_BANDWIDTH_LOCAL, ptr);

    return 0;
}
