// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (C) 2015 - 2021 Intel Corporation. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 512
#define SIZE_PTR 100

void malloc_variant(int fork_opt) {
    char *ptr[SIZE_PTR] = {NULL};
    fprintf(stderr, "\nmalloc variant %d", fork_opt);
    int i;
    for (i=0; i < SIZE_PTR;++i) {
        ptr[i] =(char*)malloc(SIZE*sizeof(char));
    }

     for (i=0; i < SIZE_PTR;++i) {
        memset(ptr[i], 0, SIZE*sizeof(char));
    }

    for (i=0; i < SIZE_PTR;++i) {
        free(ptr[i]);
    }
}

int main(int argc, char *argv[])
{
    int p = fork();
    malloc_variant(p);
    return 0;
}
