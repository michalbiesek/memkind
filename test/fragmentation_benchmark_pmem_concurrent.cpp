#include <memkind.h>
#include <cstdio>
#include <random>
#include <string>
#include <pthread.h>
#include <atomic>
#include <queue>

#define KB 1024ULL
#define MB 1048576ULL
#define GB 1073741824ULL
#define ALLOCATE_PRINT_FREQ 10000
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// block size to be allocated range from 100KB to 16MB
static const size_t block_size[] = {100 * KB, 112 * KB, 128 * KB, 160 * KB, 192 * KB,
        224 * KB, 256 * KB, 320 * KB, 384 * KB, 448 * KB, 512 * KB, 640 * KB,
        768 * KB, 896 * KB, 1 * MB, 1280 * KB, 1536 * KB, 1792 * KB, 2 * MB,
        2560 * KB, 3 * MB, 3584 * KB, 4 * MB, 5 * MB, 6 * MB, 7 * MB, 8 * MB, 15 * MB, 16 * MB};

// For debug
//static const size_t block_size[] = {128 * MB, 256 * MB, 512 * MB};

struct memkind *pmem_kind = nullptr;
size_t pmem_size = 0;

pthread_mutex_t queue_lock;

class Block
{
    public:
        char *pmem_str;
        size_t allocate_size;
};

std::queue<Block> pmem_blocks;
std::atomic_ullong total_allocated{0};
std::atomic_ullong real_allocated{0};
std::atomic_ullong allocate_print_iter{0};
std::atomic_long retry_times{0};

void release(int thread_id, char *source)
{
        if (!pmem_blocks.empty())
        {
                pthread_mutex_lock(&queue_lock);
                Block block_to_release = pmem_blocks.front();
                pmem_blocks.pop();
                char *pmem_str = block_to_release.pmem_str;
                size_t allocated_size = block_to_release.allocate_size;
                size_t evict_size = memkind_malloc_usable_size(pmem_kind, pmem_str);
                memkind_free(pmem_kind, pmem_str);

                real_allocated -= allocated_size;
                total_allocated -= evict_size;
                pthread_mutex_unlock(&queue_lock);
        }
        else
        {
                fprintf(stdout, "No blocks pending to release\n");
        }
}

void allocate(int thread_id, size_t size)
{
    char *pmem_str = nullptr;
    while((pmem_str = static_cast<char *>(memkind_malloc(pmem_kind, size))) == nullptr)
    {
        release(thread_id, (char*)"failure_cleaner");
        retry_times++;
    }

    allocate_print_iter++;

    pthread_mutex_lock(&queue_lock);
    Block block_to_allocate;
    block_to_allocate.pmem_str = pmem_str;
    block_to_allocate.allocate_size = size;
    size_t usable_size = memkind_malloc_usable_size(pmem_kind, pmem_str);
    pmem_blocks.push(block_to_allocate);

    real_allocated += size;
    total_allocated += usable_size;
    pthread_mutex_unlock(&queue_lock);

    if (allocate_print_iter % ALLOCATE_PRINT_FREQ == 0)
    {
            fprintf(stdout, "Thread Id %2d Alloc Id %d Retry Times %d Total Size %dMB added %dMB, Real Size %dMB added %dMB, external usage %.2f, internal usage %.2f, Queue Size %d\n",
                            thread_id,
                allocate_print_iter.load(),
                retry_times.load(),
                total_allocated / MB,
                usable_size / MB,
                real_allocated / MB,
                size / MB,
                static_cast<double>(total_allocated) / pmem_size,
                static_cast<double>(real_allocated) / pmem_size,
                pmem_blocks.size());
        retry_times = 0;
    }
}

static void usage(char *name)
{
	fprintf(stderr, "Usage: %s pmem_dir_path pmem_size, pmem_policy, allocate_thread_num, release_thread_num\n", name);
}

void * allocate_task(void* thread_id)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> m_size(0, ARRAY_SIZE(block_size) - 1);

    int tid = *((int*)thread_id);
    while (true)
    {
        int index = m_size(mt);
        size_t size = block_size[index];
        allocate(tid, size);
    }
    pthread_exit(NULL);
}

void * release_task(void* thread_id)
{
    int tid = *((int*)thread_id);
    while (true) {
        if (total_allocated > pmem_size * 0.9)
        {
            release(tid, (char*)"release_thread");
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    char *pmem_dir = nullptr;
    unsigned long pmem_policy = 0;
    size_t allocate_thread_num = 0;
    size_t release_thread_num = 0;
    memkind_config *pmem_cfg = nullptr;

    int err = 0;
    void *status = nullptr;

    if (argc != 6)
    {
        usage(argv[0]);
        exit;
    }
    else
    {
        // parse the arguments
        pmem_dir = argv[1];
        pmem_size = std::stoull(argv[2]) * GB;
        pmem_policy = std::stoul(argv[3]);
        allocate_thread_num = std::stoul(argv[4]);
        release_thread_num = std::stoul(argv[5]);
    }

    // memkind initialize
    pmem_cfg = memkind_config_new();
    memkind_config_set_path(pmem_cfg, pmem_dir);
    memkind_config_set_size(pmem_cfg, pmem_size);
    memkind_config_set_memory_usage_policy(pmem_cfg, static_cast<memkind_mem_usage_policy>(pmem_policy));

    err = memkind_create_pmem_with_config(pmem_cfg, &pmem_kind);
        if (err) {
            fprintf(stderr, "Unable to create pmem kind.\n");
            memkind_config_delete(pmem_cfg);
        return 1;
    }

    if (pthread_mutex_init(&queue_lock, NULL) != 0)
    {
        fprintf(stderr, "ERROR: mutex init has failed\n");
        return 1;
    }
    // start allocate thread
    pthread_t allocate_thread_ids[allocate_thread_num];
    int allocate_index[allocate_thread_num];
    for(int i = 0; i < allocate_thread_num; i++)
    {
        allocate_index[i] = i;
        pthread_create(&allocate_thread_ids[i], NULL, allocate_task, (void *)&(allocate_index[i]));
    }
    // start release thread
    pthread_t release_thread_ids[release_thread_num];
    int release_thread_index[release_thread_num];
    for(int i = 0; i < release_thread_num; i++)
    {
            release_thread_index[i] = i;
            pthread_create(&release_thread_ids[i], NULL, release_task, (void *)&(release_thread_index[i]));
    }

    // stop allocate thread
    for(int i = 0; i < allocate_thread_num; i++)
    {
        pthread_join(allocate_thread_ids[i], &status);
    }

    // stop release thread
    for(int i = 0; i < release_thread_num; i++)
    {
            pthread_join(release_thread_ids[i], &status);
    }

    pthread_mutex_destroy(&queue_lock);
    pthread_exit(NULL);
    fprintf(stdout, "Run to the end!\n");
    return 0;

}
