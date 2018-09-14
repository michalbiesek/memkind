/*
 * Copyright (c) 2015 - 2018 Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY LOG OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <memkind.h>

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#define PMEM_MAX_SIZE (MEMKIND_PMEM_MIN_SIZE * 2)

#define CHUNK_SIZE (4 * 1024 * 1024) /* assume 4MB chunks */

#define ALLOCATION_NUMBER 100


int main(int argc, char *argv[])
{
    struct memkind *pmem_kind;
    int err = 0;
    size_t i = 0;
    size_t size_test = 4096;
//    size_t allignment = 4096;
    size_t allignment = 2048;
//    size_t allignment = 8192;
    void *test[ALLOCATION_NUMBER] = {NULL};
    /* create PMEM partition */
    err = memkind_create_pmem("/mnt/pmem/", PMEM_MAX_SIZE, &pmem_kind);
    if (err) {
        perror("memkind_create_pmem()");
        fprintf(stderr, "Unable to create pmem partition\n");
        return errno ? -errno : 1;
    }


    memkind_posix_memalign(pmem_kind, &test[i],allignment,size_test);

    memkind_destroy_kind(pmem_kind);

    return 0;
}
