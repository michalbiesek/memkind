// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2021 Intel Corporation. */

#include <memkind_memtier.h>

#include <memkind/internal/memkind_arena.h>
#include <memkind/internal/memkind_log.h>

#include <assert.h>

// Provide translation from memkind_t to memtier_t
// memkind_t partition id -> memtier tier
struct memtier_registry {
    struct memtier_tier *kind_map[MEMKIND_MAX_KIND];
    pthread_mutex_t lock;
};

static struct memtier_registry memtier_registry_g = {
    {NULL},
    PTHREAD_MUTEX_INITIALIZER,
};

MEMKIND_EXPORT struct memtier_tier *memtier_tier_new(memkind_t kind)
{
    if (pthread_mutex_lock(&memtier_registry_g.lock) != 0)
        assert(0 && "failed to acquire mutex");
    if (!kind || memtier_registry_g.kind_map[kind->partition]) {
        log_err("Kind is empty or tier with kind already exists.");
        if (pthread_mutex_unlock(&memtier_registry_g.lock) != 0)
            assert(0 && "failed to release mutex");
        return NULL;
    }

    struct memtier_tier *tier = jemk_malloc(sizeof(*tier));
    if (tier) {
        tier->kind = kind;
        memtier_registry_g.kind_map[kind->partition] = tier;
    }
    if (pthread_mutex_unlock(&memtier_registry_g.lock) != 0)
        assert(0 && "failed to release mutex");
    return tier;
}

MEMKIND_EXPORT void memtier_tier_delete(struct memtier_tier *tier)
{
    if (tier) {
        if (pthread_mutex_lock(&memtier_registry_g.lock) != 0)
            assert(0 && "failed to acquire mutex");
        memtier_registry_g.kind_map[tier->kind->partition] = NULL;
        if (pthread_mutex_unlock(&memtier_registry_g.lock) != 0)
            assert(0 && "failed to release mutex");
    }
    jemk_free(tier);
}

MEMKIND_EXPORT struct memtier_builder *memtier_builder(void)
{
    return jemk_calloc(1, sizeof(struct memtier_builder));
}

MEMKIND_EXPORT int memtier_builder_add_tier(struct memtier_builder *builder,
                                            struct memtier_tier *tier,
                                            unsigned tier_ratio)
{
    if (!tier) {
        log_err("Tier is empty.");
        return -1;
    }

    struct memtier_tier_cfg *cfg =
        jemk_realloc(builder->cfg, sizeof(*cfg) * (builder->size + 1));

    if (!cfg) {
        log_err("realloc() failed.");
        return -1;
    }

    builder->cfg = cfg;
    builder->cfg[builder->size].tier = tier;
    builder->cfg[builder->size].tier_ratio = tier_ratio;
    builder->size += 1;
    return 0;
}

MEMKIND_EXPORT int memtier_builder_set_policy(struct memtier_builder *builder,
                                              memtier_policy_t policy)
{
    // TODO provide setting policy logic
    if (policy == MEMTIER_DUMMY_VALUE) {
        builder->policy = policy;
        return 0;
    }
    log_err("Unrecognized memory policy %u", policy);
    return -1;
}

static inline memkind_t get_memtier_kind(const struct memtier_kind *tier_kind)
{
    // TODO now assign first memory kind - fix it with policy logic
    return tier_kind->builder->cfg[0].tier->kind;
}

MEMKIND_EXPORT int
memtier_builder_construct_kind(struct memtier_builder *builder,
                               struct memtier_kind **kind)
{
    *kind = (struct memtier_kind *)jemk_malloc(sizeof(*kind));
    if (*kind) {
        (*kind)->builder = builder;
        return 0;
    }
    log_err("malloc() failed.");
    return -1;
}

MEMKIND_EXPORT void memtier_delete_kind(struct memtier_kind *kind)
{
    jemk_free(kind->builder);
    jemk_free(kind);
}

MEMKIND_EXPORT void *memtier_kind_malloc(struct memtier_kind *kind, size_t size)
{
    // TODO provide tiering_kind logic
    return memkind_malloc(get_memtier_kind(kind), size);
}

MEMKIND_EXPORT void *memtier_tier_malloc(struct memtier_tier *tier, size_t size)
{
    void *ptr = memkind_malloc(tier->kind, size);
    tier->alloc_size += jemk_malloc_usable_size(ptr);
    return ptr;
}

MEMKIND_EXPORT void *memtier_kind_calloc(struct memtier_kind *kind, size_t num,
                                         size_t size)
{
    // TODO provide tiering_kind logic
    return memkind_calloc(get_memtier_kind(kind), num, size);
}

MEMKIND_EXPORT void *memtier_tier_calloc(struct memtier_tier *tier, size_t num,
                                         size_t size)
{
    void *ptr = memkind_calloc(tier->kind, num, size);
    tier->alloc_size += jemk_malloc_usable_size(ptr);
    return ptr;
}

MEMKIND_EXPORT void *memtier_kind_realloc(struct memtier_kind *kind, void *ptr,
                                          size_t size)
{
    // TODO provide tiering_kind logic
    return memkind_realloc(get_memtier_kind(kind), ptr, size);
}

MEMKIND_EXPORT void *memtier_tier_realloc(struct memtier_tier *tier, void *ptr,
                                          size_t size)
{
    if (size == 0 && ptr != NULL) {
        tier->alloc_size -= jemk_malloc_usable_size(ptr);
        memkind_free(tier->kind, ptr);
        return NULL;
    } else if (ptr == NULL) {
        void *n_ptr = memkind_malloc(tier->kind, size);
        tier->alloc_size += jemk_malloc_usable_size(ptr);
        return n_ptr;
    } else {
        tier->alloc_size -= jemk_malloc_usable_size(ptr);
        void *n_ptr = memkind_realloc(tier->kind, ptr, size);
        tier->alloc_size += jemk_malloc_usable_size(n_ptr);
        return n_ptr;
    }
}

MEMKIND_EXPORT int memtier_kind_posix_memalign(struct memtier_kind *kind,
                                               void **memptr, size_t alignment,
                                               size_t size)
{
    // TODO provide tiering_kind logic
    return memkind_posix_memalign(get_memtier_kind(kind), memptr, alignment,
                                  size);
}

MEMKIND_EXPORT int memtier_tier_posix_memalign(struct memtier_tier *tier,
                                               void **memptr, size_t alignment,
                                               size_t size)
{
    int res = memkind_posix_memalign(tier->kind, memptr, alignment, size);
    tier->alloc_size += jemk_malloc_usable_size(memptr);
    return res;
}

MEMKIND_EXPORT size_t memtier_usable_size(void *ptr)
{
    return jemk_malloc_usable_size(ptr);
}

MEMKIND_EXPORT void memtier_free(void *ptr)
{
    struct memkind *kind = memkind_detect_kind(ptr);
    if (!kind)
        return;
    struct memtier_tier *tier = memtier_registry_g.kind_map[kind->partition];
    tier->alloc_size -= jemk_malloc_usable_size(ptr);
    memkind_free(kind, ptr);
}