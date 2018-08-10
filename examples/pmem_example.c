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
#include <memkind/internal/memkind_pmem.h>

#include <sys/param.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

static const size_t PMEM_PART_SIZE_EXAMPLE = MEMKIND_PMEM_MIN_SIZE + 4096;
static const char*  PMEM_DIR_EXAMPLE = "/mnt/mem/";

int
main(int argc, char *argv[])
{
    struct memkind *pmem_temp = NULL;
    const size_t pmem_number = 2000;
    const size_t alloc_value_fail = 14336;
    const size_t alloc_value_good= 14337;
    const size_t alloc_value_huge = MEMKIND_PMEM_CHUNK_SIZE;
    (void) alloc_value_fail;
    (void) alloc_value_good;
    (void) alloc_value_huge;
    void * temp = NULL;
    for (unsigned int i = 0; i < pmem_number; ++i ) {
        int err = memkind_create_pmem(PMEM_DIR_EXAMPLE, PMEM_PART_SIZE_EXAMPLE, &pmem_temp);
        if ( err )
        {
            fprintf(stderr, "Unable to memkind_create_pmem %u\n",i);
            return errno ? -errno : 1;
        }
        temp = memkind_malloc(pmem_temp, alloc_value_good); //replace with alloc_value_fail to fail
        if ( !temp )
        {
            fprintf(stderr, "Unable to malloc\n");
            return errno ? -errno : 1;
        }
        memkind_free(pmem_temp,temp);
        err = memkind_destroy_kind(pmem_temp);
        if ( err )
        {
            fprintf(stderr, "Unable to destory kind %u\n",i);
            return errno ? -errno : 1;
        }
        if ( i %10 == 0)
        {
            fprintf(stdout, "\nLoop value of i %d",i);
        }
    }
    fprintf(stdout, "\nLoop ended test pass");
    return 0;
}
