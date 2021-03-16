// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2021 Intel Corporation. */

#include <stdlib.h>

int main(void)
{
    void *ptr = malloc(100);
    void *ptr2 = calloc(1, 200);
    ptr = realloc(ptr, 120);
    free(ptr);
    free(ptr2);

    return 0;
}
