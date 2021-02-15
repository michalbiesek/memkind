#ifndef JEMALLOC_INTERNAL_ESET_H
#define JEMALLOC_INTERNAL_ESET_H

#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/fb.h"
#include "jemalloc/internal/edata.h"
#include "jemalloc/internal/mutex.h"

/*
 * An eset ("extent set") is a quantized collection of extents, with built-in
 * LRU queue.
 *
 * This class is not thread-safe; synchronization must be done externally if
 * there are mutating operations.  One exception is the stats counters, which
 * may be read without any locking.
 */
typedef struct eset_s eset_t;
struct eset_s {
	/* Quantized per size class heaps of extents. */
	edata_heap_t heaps[SC_NPSIZES + 1];
	atomic_zu_t nextents[SC_NPSIZES + 1];
	atomic_zu_t nbytes[SC_NPSIZES + 1];

	/* Bitmap for which set bits correspond to non-empty heaps. */
	fb_group_t bitmap[FB_NGROUPS(SC_NPSIZES + 1)];

	/* LRU of all extents in heaps. */
	edata_list_inactive_t lru;

	/* Page sum for all extents in heaps. */
	atomic_zu_t npages;

	/*
	 * A duplication of the data in the containing ecache.  We use this only
	 * for assertions on the states of the passed-in extents.
	 */
	extent_state_t state;
};

void eset_init(eset_t *eset, extent_state_t state);

size_t eset_npages_get(eset_t *eset);
/* Get the number of extents in the given page size index. */
size_t eset_nextents_get(eset_t *eset, pszind_t ind);
/* Get the sum total bytes of the extents in the given page size index. */
size_t eset_nbytes_get(eset_t *eset, pszind_t ind);

void eset_insert(eset_t *eset, edata_t *edata);
void eset_remove(eset_t *eset, edata_t *edata);
/*
 * Select an extent from this eset of the given size and alignment.  Returns
 * null if no such item could be found.
 */
edata_t *eset_fit(eset_t *eset, size_t esize, size_t alignment, bool exact_only,
    unsigned lg_max_fit);

#endif /* JEMALLOC_INTERNAL_ESET_H */
