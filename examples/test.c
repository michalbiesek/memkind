/*
 * Copyright (c) 2018 - 2019 Intel Corporation
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
#include <memkind/internal/memkind_private.h>

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#define PMEM_MAX_SIZE (1024 * 1024 * 32)
static char *PMEM_DIR = "/tmp/";

int main()
{
	struct memkind *kind = NULL;

	int err = memkind_create_pmem(PMEM_DIR, PMEM_MAX_SIZE, &kind);
	if (err != 0)
		printf("err = %d\n", err);

	int bin = 0;
	int run = 0;
	int hint = -1;
	void *ptr = memkind_malloc(kind, 32);
	hint = je_get_defrag_hint(ptr, &bin, &run);
	
	printf("\n%d, %d, %d\n", hint, bin, run);

	memkind_free(kind, ptr);
	memkind_destroy_kind(kind);

	return 0;
}

