// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2020 Intel Corporation. */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <memkind.h>
#include "memkind_default.h"
#include "memkind_arena.h"


/*
 * Header file for the file-backed memory memkind operations.
 * More details in memkind_pmem(3) man page.
 *
 * Functionality defined in this header is considered as EXPERIMENTAL API.
 * API standards are described in memkind(3) man page.
 */


struct memkind_user {
    int init_err;
    int num_cpu;
    struct vec_cpu_node *closest_numanode;
};

int memkind_user_create(struct memkind *kind);

#ifdef __cplusplus
}
#endif
