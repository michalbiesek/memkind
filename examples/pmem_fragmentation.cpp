#include <memkind.h>

#include <memory>
#include <cstring>
#include <stdio.h>
#include <random>
#include <iostream>
#include <malloc.h>
#include <vector>
#include <stdlib.h>
#include <stdlib.h>


#define BUFFER_SIZE 1024
#define PMEM_MAX_SIZE ((size_t)1024ULL * 1024 * 1024 * 200)

int main(int argc, char * argv[])
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<> m_size(0, 9);
	//std::uniform_int_distribution<> m_index(0, 10);

	double block_size[] = {8519680, 4325376, 8519680, 4325376, 8519680, 4325376, 8519680, 4325376, 432517, 608478};

        struct memkind *pmem_kind;
        int err = 0;

        err = memkind_create_pmem("/mnt/pmem1", PMEM_MAX_SIZE, &pmem_kind);
        if (err)
        {
                printf("%s\n", "Unable to create pmem partition\n");
                return -1;
        }
        err =  memkind_update_memory_usage_policy(pmem_kind, MEMKIND_MEM_USAGE_POLICY_CONSERVATIVE);
        if (err)
        {
                printf("%s\n", "Unable to set pmem memory usage policy\n");
                return -1;
        }

        char *pmem_str = NULL;

//        bool full = false;
//	int evict = 0;
//        long long malloc_sum = 0;

	std::vector<char*> pmem_strs;
	long long total = PMEM_MAX_SIZE;
        size_t total_allocated = 0;
        long long n = 0;

        while (true)
        {
                n++;
		int index = m_size(mt);
		size_t size = block_size[index];
                int length = pmem_strs.size() / 2;
                std::uniform_int_distribution<> m_index(0, length-1);
                while ((pmem_str = (char *)memkind_malloc(pmem_kind, size)) == NULL) {
                        int to_evict = m_index(mt);
                        char *str_to_evict = pmem_strs[to_evict];
                        long long evict_size = memkind_malloc_usable_size(pmem_kind, str_to_evict);
                        total_allocated -= evict_size;
                        if (total_allocated < total * 0.1) {
                                printf("allocated less than 10 percent\n");
                                exit(0);
                        }
                        memkind_free(pmem_kind, str_to_evict);
                        pmem_strs.erase(pmem_strs.begin() + to_evict);
                }
                pmem_strs.push_back(pmem_str);
                total_allocated += memkind_malloc_usable_size(pmem_kind, pmem_str);
                if (n % 1000000 == 0) {
                        printf("%f\n", (float)total_allocated/total );
			fflush(stdout);
                }

		//system("df -k | grep pmem0");
        }
        //printf("%d\t\t%d\n", PMEM_MAX_SIZE / 1024 / 1024 / 1024, malloc_sum / 1024 / 1024 / 1024);

	printf("Out of memory");
        return 0;
}
