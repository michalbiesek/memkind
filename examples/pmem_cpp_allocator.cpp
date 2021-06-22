// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (C) 2021 Intel Corporation. */

#include "memkind.h"

#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <scoped_allocator>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <random>
#include <string.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <random>

#define KB (1024ULL)
#define MB (1024ULL * (KB))
#define GB (1024ULL * (MB))
#define PMEM_SIZE (10 * (GB))
const size_t SIZE_1 = 4096;
const size_t SIZE_2 = 64;
const size_t SIZE_3 = 1024;
const size_t SIZE_4 = 2048;

static struct memkind *pmem_kind;
static std::vector<void *> pmem_vec;
auto rng = std::default_random_engine {};
std::uniform_int_distribution<> distr_1(SIZE_2, SIZE_1);
std::uniform_int_distribution<> distr_2(SIZE_3, SIZE_4);

void alloc_pattern(size_t size_min, size_t size_max)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(size_min, size_max); // define the range

    size_t total_alloc_size = 0;
    size_t size;
    size_t i;
    size_t count_defrag_realloc = 0;
    while(1) {
        size = distr(gen);
        void *ptr = memkind_malloc(pmem_kind, size);
        if (!ptr)
            break;
        memset(ptr, 'a', size);
        total_alloc_size += memkind_malloc_usable_size(pmem_kind, ptr);
        pmem_vec.push_back(ptr);
    }
    std::cout <<  "Vector allocation size min " << size_min << std::endl;
    std::cout <<  "Vector allocation size max " << size_max << std::endl;

    std::cout <<  "Total allocated size before defrag " << total_alloc_size << std::endl;
    std::cout <<  "Total allocation no before defrag " << pmem_vec.size() << std::endl;


    for (i = 0; i < pmem_vec.size(); ++i) {
        void *new_ptr = memkind_defrag_reallocate(pmem_kind, pmem_vec.at(i));
        if (new_ptr) {
            pmem_vec.at(i) = new_ptr;
            count_defrag_realloc++;
        }
    }

    while(1) {
        size = distr(gen);
        void *ptr = memkind_malloc(pmem_kind, size);
        if (!ptr)
            break;
        memset(ptr, 'a', size);
        total_alloc_size += memkind_malloc_usable_size(pmem_kind, ptr);
        pmem_vec.push_back(ptr);
    }

    std::cout <<  "Total allocated size after defrag " << total_alloc_size << std::endl;
    std::cout <<  "Total allocation no after defrag " << pmem_vec.size() << std::endl;
    std::cout <<  "Total Defrag no " << pmem_vec.size() << std::endl;
    std::cout <<  std::endl;
    //shuffle vector to randomize freeing order
    std::shuffle(std::begin(pmem_vec), std::end(pmem_vec), rng);

    //free memory
    for (auto const &val : pmem_vec) {
        memkind_free(pmem_kind, val);
    }
    pmem_vec.clear();
}

int main(int argc, char *argv[])
{
    const char *pmem_directory = "/tmp/";

    if (argc > 2) {
        std::cerr
            << "Usage: test_app [directory path]\n"
            << "\t[directory path] - directory where temporary file is created (default = \"/tmp/\")"
            << std::endl;
        return 0;
    } else if (argc == 2) {
        int status = memkind_check_dax_path(argv[1]);
        if (!status) {
            std::cout << "PMEM kind is on DAX-enabled File system.\n"<< argv[1]<< std::endl;
        } else {
            std::cout << "PMEM kind is not on DAX-enabled File system.\n"<< argv[1]<< std::endl;
        }
        pmem_directory = argv[1];
    }

    //reserve vector to avoid resize it
    const size_t alloc = 1500000000;
    pmem_vec.reserve(alloc);

    std::cout <<  "Size of PMEM kind: " << PMEM_SIZE << std::endl;

    int ret = memkind_create_pmem(pmem_directory, (size_t)(PMEM_SIZE), &pmem_kind);

    if (ret)
        return -1;

    for (int i = 0; i <10 ; i++)
    {
        std::cout <<  "Iteration no " << i << std::endl;
        alloc_pattern(SIZE_1, SIZE_1);
        alloc_pattern(SIZE_2, SIZE_2);
        alloc_pattern(SIZE_2, SIZE_1);
        alloc_pattern(SIZE_3, SIZE_4);
    }

    ret = memkind_destroy_kind(pmem_kind);

    return 0;
}
