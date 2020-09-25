// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2020 Intel Corporation. */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <memkind.h>

/*
 * Header file for the HMAT memory memkind operations.
 * More details in memkind_hwloc(3) man page.
 *
 * Functionality defined in this header is considered as EXPERIMENTAL API.
 * API standards are described in memkind(3) man page.
 */

extern struct memkind_ops MEMKIND_CAPACITY_OPS;

#ifdef __cplusplus
}
#endif
