/*
 * Copyright (C) 2018 Intel Corporation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice(s),
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice(s),
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
 * EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
/*
 *  Header file for the persistent memory interface.
 *
 *  This file defines the external API's and enumerations for the
 *  pmemmalloc library.  These interfaces define a heap manager that
 *  targets the high bandwidth memory numa nodes.
 *
 *  hbwmalloc.h functionality is considered as stable API (STANDARD API).
 *
 *  Please read pmemmalloc(3) man page for or more details.
 */

/* EXPERIMENTAL API */
/** The minimum size which allows to limit the file-backed memory partition */
#define MEMKIND_PMEM_MIN_SIZE (1024 * 1024 * 16)

typedef struct memkind* pmemkind_t;

int pmem_create(const char *dir, size_t max_size,
                       struct memkind **kind);

int pmem_destroy(pmemkind_t kind);

void *pmem_malloc(pmemkind_t kind, size_t size);

size_t pmem_malloc_usable_size(pmemkind_t kind, void *ptr);

void *pmem_calloc(pmemkind_t kind, size_t num, size_t size);

int pmem_posix_memalign(pmemkind_t kind, void **memptr, size_t alignment,
                        size_t size);

void *pmem_realloc(pmemkind_t kind, void *ptr, size_t size);

void pmem_free(pmemkind_t kind, void *ptr);

#ifdef __cplusplus
}
#endif
