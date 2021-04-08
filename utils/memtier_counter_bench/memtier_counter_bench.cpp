// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (C) 2021 Intel Corporation. */

#include <memkind_memtier.h>

#include <argp.h>

#include <assert.h>
#include <cstring>
#include <pthread.h>
#include <stdint.h>
#include <thread>

#include <vector>

#define SIZE         512
#define ITERATION    10000000
#define THREAD_COUNT 4

static struct argp_option options[] = {
    { "memkind", 'm', 0, 0, "Benchmark memkind."},
    { "memtier_kind", 'k', 0, 0, "Benchmark memtier_memkind."},
    { "memtier", 'r', 0, 0, "Benchmark memtier_memory."},
    { "thread", 't', 0, 0, "Threads numbers."}
};

class counter_bench_alloc {
public:
	virtual void* alloc(size_t) = 0;
	virtual void free(void*) = 0;
};

class memkind_bench_alloc: public counter_bench_alloc {
public:
    void* alloc(size_t size) {
        return memkind_malloc(MEMKIND_DEFAULT, SIZE);
    }

    void free(void* ptr) {
        memkind_free(MEMKIND_DEFAULT, ptr);
    }
};

class memtier_kind_bench_alloc: public counter_bench_alloc {
public:
    void* alloc(size_t size) {
        return memtier_kind_malloc(MEMKIND_DEFAULT, SIZE);
    }

    void free(void* ptr) {
        memtier_kind_realloc(MEMKIND_DEFAULT, ptr, 0);
    }
};

class memtier_bench_alloc: public counter_bench_alloc {
public:
	memtier_bench_alloc() {
        m_tier_builder = memtier_builder_new();
        memtier_builder_add_tier(m_tier_builder, MEMKIND_DEFAULT, 1);
        memtier_builder_construct_memtier_memory(m_tier_builder);
    }

	~memtier_bench_alloc() {
        memtier_builder_delete(m_tier_builder);
        memtier_delete_memtier_memory(m_tier_memory);
    }

    void* alloc(size_t size) {
        return memtier_malloc(m_tier_memory, size);
    }

    void free(void* ptr) {
        memtier_realloc(m_tier_memory, ptr, 0);
    }
private:

    struct memtier_builder *m_tier_builder;
    struct memtier_memory *m_tier_memory;
};

struct BenchArgs {
    counter_bench_alloc *bench;
    int thread_no;
};

int benchmark_run(size_t ntimes, size_t nthreads)
{
    std::vector<std::thread> vthread(nworks);
    for (size_t k = 0; k < nworks; ++k) {
        vthread[k] = std::thread([&]() {
            std::vector<void *> v;
            v.reserve(ntimes);
            for (size_t i = 0; i < ntimes; i++) {
                // alloc_operation
                v.push_back();
            }
            for (size_t i = 0; i < ntimes; i++) {
                // freeing operaion
                memkind_free(MEMKIND_DEFAULT, v[i]);
            }
            v.clear();
        });
    }
    for (auto &t : vthread) {
        t.join();
    }
    return 0;

}

// int memkind_bench_thread(size_t ntimes, size_t nworks)
// {
//     std::vector<std::thread> vthread(nworks);
//     for (size_t k = 0; k < nworks; ++k) {
//         vthread[k] = std::thread([&]() {
//             std::vector<void *> v;
//             v.reserve(ntimes);
//             for (size_t i = 0; i < ntimes; i++) {
//                 v.push_back(memkind_malloc(MEMKIND_DEFAULT, SIZE));
//             }
//             for (size_t i = 0; i < ntimes; i++) {
//                 memkind_free(MEMKIND_DEFAULT, v[i]);
//             }
//             v.clear();
//         });
//     }
//     for (auto &t : vthread) {
//         t.join();
//     }
//     return 0;
// }

// int memkind_bench_single(size_t ntimes)
// {
//     std::vector<void *> v;
//     v.reserve(ntimes);
//     for (size_t i = 0; i < ntimes; i++) {
//         v.push_back(memkind_malloc(MEMKIND_DEFAULT, SIZE));
//     }
//     for (size_t i = 0; i < ntimes; i++) {
//         memkind_free(MEMKIND_DEFAULT, v[i]);
//     }
//     v.clear();
//     return 0;
// }

// int memtier_bench_single(size_t ntimes)
// {
//     std::vector<void *> v;
//     v.reserve(ntimes);
//     for (size_t i = 0; i < ntimes; i++) {
//         v.push_back(memtier_kind_malloc(MEMKIND_DEFAULT, SIZE));
//     }
//     for (size_t i = 0; i < ntimes; i++) {
//         memtier_kind_realloc(MEMKIND_DEFAULT, v[i], 0);
//     }
//     v.clear();
//     return 0;
// }

// int memtier_bench_thread(size_t ntimes, size_t nworks)
// {
//     std::vector<std::thread> vthread(nworks);
//     for (size_t k = 0; k < nworks; ++k) {
//         vthread[k] = std::thread([&]() {
//             std::vector<void *> v;
//             v.reserve(ntimes);
//             for (size_t i = 0; i < ntimes; i++) {
//                 v.push_back(memtier_kind_malloc(MEMKIND_DEFAULT, SIZE));
//             }
//             for (size_t i = 0; i < ntimes; i++) {
//                 memtier_kind_realloc(MEMKIND_DEFAULT, v[i], 0);
//             }
//             v.clear();
//         });
//     }
//     for (auto &t : vthread) {
//         t.join();
//     }
//     return 0;
// }

static int parse_opt(int key, char *arg, struct argp_state *state) {
  auto args = (BenchArgs *)state->input;
  switch (key) {
  case 'm':
    args->mode = MEMKIND;
    break;
  case 'k':
    args->mode = MEMTIER_KIND;
    break;
  case 'r':
    args->mode = MEMTIER;
    break;
  }
  return 0;
}

static struct argp argp = { options, parse_opt, nullptr, nullptr, 0, 0, 0 };

int main(int argc, char *argv[])
{
   struct BenchArgs arguments;

   int res = argp_parse(&argp, argc, argv, 0, 0, &arguments);
   if (res) {

   } else {
    exit(1);
   }
    // allocator_factory.initialize_allocator(kind);

    // allocator_factory.run(kind);

    // allocator_factory.finmi(kind);

    // if (argc != 2) {
    //     exit(1);
    // }






    // if (strcmp(argv[1], "memkind_thread") == 0) {
    //     return memkind_bench_thread(ITERATION, THREAD_COUNT);
    // } else if (strcmp(argv[1], "memtier_thread") == 0) {
    //     return memtier_bench_thread(ITERATION, THREAD_COUNT);
    // } else if (strcmp(argv[1], "memkind_single") == 0) {
    //     return memkind_bench_single(ITERATION);
    // } else if (strcmp(argv[1], "memtier_single") == 0) {
    //     return memtier_bench_single(ITERATION);
    // } else {
    //     exit(1);
    }
}