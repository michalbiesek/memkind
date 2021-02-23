// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2021 Intel Corporation. */

#include <memkind.h>
#include <memkind/internal/memkind_private.h>
#include <memtier.h>

#include <stdlib.h>

MEMKIND_EXPORT struct tiering_class *memtier_cls_new(void)
{
    return malloc(sizeof(struct tiering_class));
}

MEMKIND_EXPORT void memtier_cls_delete(struct tiering_class *cls)
{
    free(cls);
}

MEMKIND_EXPORT int memtier_cls_set_ratio(struct tiering_class *cls,
                                         size_t ratio)
{
    cls->ratio = ratio;
    return MEMKIND_SUCCESS;
}

MEMKIND_EXPORT int memtier_cls_set_memory_kind(struct tiering_class *cls,
                                               memkind_t kind)
{
    cls->kind = kind;
    return MEMKIND_SUCCESS;
}

MEMKIND_EXPORT struct tiering_config *memtier_config_new()
{
    return malloc(sizeof(struct tiering_config));
}

MEMKIND_EXPORT int memtier_config_add_tiering_class(struct tiering_config *cfg,
                                                    struct tiering_class cls)
{
    // TODO provide adding class logic
    return 0;
}

MEMKIND_EXPORT int memtier_config_set_policy(struct tiering_config *cfg,
                                             memtier_policy_t policy)
{
    // TODO provide setting policy logic
    return 0;
}

MEMKIND_EXPORT int memtier_new_kind_with_config(struct tiering_config *cfg,
                                                struct tiering_kind **kind)
{
    *kind = malloc(sizeof(struct tiering_kind));
    return 0;
}

MEMKIND_EXPORT void memtier_delete_kind(struct tiering_kind *kind)
{
    free(kind);
}

MEMKIND_EXPORT void *memtier_kind_malloc(struct tiering_kind *kind, size_t size)
{
    // TODO provide tiering_kind logic
    return NULL;
}

MEMKIND_EXPORT void *memtier_cls_malloc(struct tiering_class *cls, size_t size)
{
    // TODO provide increment counter logic
    return memkind_malloc(cls->kind, size);
}

MEMKIND_EXPORT void *memtier_kind_calloc(struct tiering_kind *kind, size_t num,
                                         size_t size)
{
    // TODO provide tiering_kind logic
    return NULL;
}

MEMKIND_EXPORT void *memtier_cls_calloc(struct tiering_class *cls, size_t num,
                                        size_t size)
{
    // TODO provide increment counter logic
    return memkind_calloc(cls->kind, num, size);
}

MEMKIND_EXPORT int memtier_kind_posix_memalign(struct tiering_kind *kind,
                                               void **memptr, size_t alignment,
                                               size_t size)
{
    // TODO provide tiering_kind logic
    return 0;
}

MEMKIND_EXPORT int memtier_cls_posix_memalign(struct tiering_class *cls,
                                              void **memptr, size_t alignment,
                                              size_t size)
{
    // TODO provide increment counter logic
    return memkind_posix_memalign(cls->kind, memptr, alignment, size);
}

MEMKIND_EXPORT void *memtier_kind_realloc(struct tiering_kind *kind, void *ptr,
                                          size_t size)
{
    // TODO provide tiering_kind logic
    return NULL;
}

MEMKIND_EXPORT void *memtier_cls_realloc(struct tiering_class *cls, void *ptr,
                                         size_t size)
{
    // TODO provide increment/decrement counter logic
    if (ptr) {
        return NULL;
    } else {
        return NULL;
    }
}

MEMKIND_EXPORT size_t memtier_usable_size(void *ptr)
{
    return memkind_malloc_usable_size(NULL, ptr);
}

MEMKIND_EXPORT void memtier_free(void *ptr)
{
    struct memkind *kind = memkind_detect_kind(ptr);
    // TODO detect class from which kind belongs
    // TODO provide increment/decrement counter logic
    memkind_free(kind, ptr);
}
