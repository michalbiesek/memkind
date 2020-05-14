// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2019 - 2020 Intel Corporation. */

#include <memkind.h>
#include <stdio.h>
#define LOOP_SIZE 100000
int main()
{
    const size_t size = 1024;
    int i;
    int j;
    
    char *ptr_regular = (char *)memkind_malloc(MEMKIND_REGULAR, size);

    fprintf(stdout,
            "This example shows how to use memkind API to retrieve information about allocation stats.\n");

    snprintf(ptr_regular, size,
             "Hello world from regular kind memory - ptr_regular.\n");

    for (j=0; j <LOOP_SIZE; ++j) {
        for (i=0; i <LOOP_SIZE; ++i) {
            size_t size = memkind_malloc_usable_size(NULL, ptr_regular);
            (void)size;
        }
    }

    return 0;
}
