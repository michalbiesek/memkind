#include <memkind/internal/memkind_user.h>

static int memkind_dax_kmem_check_available(struct memkind *kind)
{
    return kind->ops->get_mbind_nodemask(kind, NULL, 0);
}

static int memkind_user_get_mbind_nodemask(struct memkind *kind, unsigned long *nodemask, unsigned long maxnode)
{
    struct memkind_user *priv = kind->priv;
    if (MEMKIND_LIKELY(!priv->init_err)) {
        priv->init_err = set_bitmask_for_current_closest_numanode(nodemask, maxnode,
                                                               priv->closest_numanode, priv->num_cpu);
    }
    return priv->init_err;
}

int memkind_user_create(struct memkind *kind)
{
    int err;

    struct memkind *priv = (struct memkind_user *)malloc(sizeof(struct memkind_user));
    if (!priv) {
        log_err("malloc() failed.");
        return MEMKIND_ERROR_MALLOC;
    }

    err = memkind_default_create(kind, ops, name);
    if (err) {
        goto exit;
    }

    err = memkind_arena_create_map(kind, &pmem_extent_hooks);
    if (err) {
        goto exit;
    }

    kind->priv = priv;
    return 0;

exit:
    free(priv);
    return err;
}

EMKIND_EXPORT struct memkind_ops MEMKIND_USER_OPS = {
    .create = memkind_arena_create,
    .destroy = memkind_default_destroy,
    .malloc = memkind_arena_malloc,
    .calloc = memkind_arena_calloc,
    .posix_memalign = memkind_arena_posix_memalign,
    .realloc = memkind_arena_realloc,
    .free = memkind_arena_free,
    .check_available = memkind_user_check_available,
    .mbind = memkind_default_mbind,
    .get_mmap_flags = memkind_default_get_mmap_flags,
    .get_mbind_mode = memkind_default_get_mbind_mode,
    .get_mbind_nodemask = memkind_user_get_mbind_nodemask,
    .get_arena = memkind_thread_get_arena,
    .malloc_usable_size = memkind_default_malloc_usable_size,
    .finalize = memkind_arena_finalize,
    .get_stat = memkind_arena_get_kind_stat,
    .defrag_reallocate = memkind_arena_defrag_reallocate
};