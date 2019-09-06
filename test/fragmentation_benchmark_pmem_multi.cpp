/*
 * Copyright (C) 2019 Intel Corporation.
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

#include <memkind.h>

#include <iostream>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <random>
#include <thread>
#include <vector>

#define KB (1024)
#define MB (KB * 1024)
#define PRINT_FREQ 10000
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define BENCHMARK_THREAD_LOG  "bench_multi_thread_%u_"
#define BENCHMARK_LOG "%Y%m%d-%H%M.log"

typedef struct thread_params {
    FILE * output_file;
    size_t min_size;
    size_t max_size;
    size_t pmem_size;
    size_t iteration;
} thread_params;

static struct memkind *pmem_kind;

static void usage(char *name)
{
    fprintf(stderr,
            "Usage: %s pmem_kind_dir_path pmem_size pmem_policy min_size max_size iterations thread_number\n",
            name);
}

static void thread_func(thread_params *params)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::default_random_engine generator;
    std::uniform_int_distribution<size_t> distribution(params->min_size, params->max_size);
    char *pmem_str = nullptr;
    std::vector<char *> pmem_strs;
    size_t total_size = params->pmem_size;
    size_t total_allocated = 0;
    size_t iteration = params->iteration;
    do {
        size_t size = distribution(mt);
        while ((pmem_str = static_cast<char *>(memkind_malloc(pmem_kind,
                                                              size))) == nullptr) {
            //evict first item
            char *str_to_evict = pmem_strs.front();
            size_t evict_size = memkind_malloc_usable_size(pmem_kind, str_to_evict);
            total_allocated -= evict_size;

            if (total_allocated < total_size * 0.1) {
                fprintf(stderr,"allocated less than 10 percent\n");
            }
            memkind_free(pmem_kind, str_to_evict);
            pmem_strs.erase(pmem_strs.begin());
        }
        pmem_strs.push_back(pmem_str);
        total_allocated += memkind_malloc_usable_size(pmem_kind, pmem_str);

        if (iteration % PRINT_FREQ == 0) {
            fprintf(params->output_file,"%f\n", static_cast<double>(total_allocated) / total_size);
            fflush(stdout);
        }
        iteration --;
    } while (iteration != 0);
    //cleanup rest
    for (auto it = pmem_strs.begin(); it != pmem_strs.end(); ) {
        memkind_free(pmem_kind, *it);
        it = pmem_strs.erase(it);
    }
}

static int create_pmem(const char *pmem_dir, size_t pmem_size,
                       memkind_mem_usage_policy policy)
{
    int err = 0;
    if (pmem_size == 0 ) {
        fprintf(stderr, "Invalid size to pmem kind must be not equal zero.\n");
        return 1;
    }

    if (policy > MEMKIND_MEM_USAGE_POLICY_MAX_VALUE) {
        fprintf(stderr, "Invalid memory usage policy param %u.\n", policy);
        return 1;
    }

    memkind_config *pmem_cfg = memkind_config_new();
    if (!pmem_cfg) {
        fprintf(stderr, "Unable to create pmem configuration.\n");
        return 1;
    }

    memkind_config_set_path(pmem_cfg, pmem_dir);
    memkind_config_set_size(pmem_cfg, pmem_size);
    memkind_config_set_memory_usage_policy(pmem_cfg, policy);

    err = memkind_create_pmem_with_config(pmem_cfg, &pmem_kind);
    memkind_config_delete(pmem_cfg);
    if (err) {
        fprintf(stderr, "Unable to create pmem kind.\n");
        return 1;
    }

    return 0;
}

static thread_params * create_thread_param(size_t thread_no, size_t iteration, size_t pmem_size, size_t min_size, size_t max_size)
{
    unsigned i = 0;
    thread_params *threads_params = new thread_params[thread_no];
    for (i = 0; i < thread_no; ++i) {
        char file_name[100] = {'\0'};
        auto result = std::time(nullptr);
        int size =snprintf(file_name, 100, BENCHMARK_THREAD_LOG, i);
        strftime(file_name + size, 100, BENCHMARK_LOG, std::localtime(&result));
        threads_params[i].output_file = fopen(file_name, "w+");
        if (threads_params[i].output_file == nullptr)
        {
            fprintf(stderr, "Cannot create output file %s\n", file_name);
            exit(1);
        }
        threads_params[i].min_size = min_size;
        threads_params[i].max_size = max_size;
        threads_params[i].pmem_size = pmem_size;
        threads_params[i].iteration = iteration;
    }
    return threads_params;
}

static void destroy_thread_param(thread_params *threads_params, size_t thread_no)
{
    unsigned i = 0;
    for (i = 0; i < thread_no; ++i) {
        fclose(threads_params[i].output_file);
    }
    delete [] threads_params;
}

int main(int argc, char *argv[])
{
    char *pmem_dir;
    size_t pmem_size, min_size, max_size, iteration, thread_no;
    memkind_mem_usage_policy pmem_policy;
    int err = 0;
    int status = 0;
    unsigned i = 0;

    if (argc != 8) {
        usage(argv[0]);
        return 1;
    } else {
        pmem_dir = argv[1];
        pmem_size = std::stoull(argv[2]) * MB;
        pmem_policy = static_cast<memkind_mem_usage_policy>(std::stoul(argv[3]));
        min_size = std::stoull(argv[4]) * KB;
        max_size = std::stoull(argv[5]) * KB;
        iteration = std::stoull(argv[6]);
        thread_no = std::stoull(argv[7]);
    }

    err= create_pmem(pmem_dir, pmem_size, pmem_policy);
    if (err) {
        fprintf(stderr, "Cannot create pmem.\n");
        return 1;
    }

    std::thread *threads = new std::thread[thread_no];
    thread_params *threads_params = create_thread_param(thread_no, iteration, pmem_size, min_size, max_size);
    for (i = 0; i < thread_no; ++i) {
        threads[i] = std::thread(thread_func, &threads_params[i]);
    }

    for (i = 0; i < thread_no; ++i) {
        threads[i].join();
    }

    err = memkind_destroy_kind(pmem_kind);
    if (err) {
        fprintf(stdout, "Unable to destroy pmem kind.\n");
    }

    destroy_thread_param(threads_params, thread_no);

    delete [] threads;

    return status;
}
