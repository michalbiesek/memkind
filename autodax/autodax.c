#include <memkind.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define AUTODAX_EXPORT __attribute__((visibility("default")))
#define AUTODAX_INIT __attribute__((constructor))


// Following is the type of HBW memory that is allocated using memkind.
// By changing this type, this library can be used to allocate other
// types of memory types (e.g., MEMKIND_HUGETLB, MEMKIND_GBTLB,
// MEMKIND_HBW_HUGETLB etc.)
static memkind_t memkind_kind;


// This function is executed at library load time.
// Initialize HBW arena by making a dummy allocation/free at library load
// time. Until HBW initialization is complete, we must not call any
// allocation routines with HBW as kind.
static void AUTODAX_INIT autohbw_load(void)
{
    // First set the default memory type this library allocates. This can
    // be overridden by env variable
    // Note: 'memkind_hbw_preferred' will allow falling back to DDR but
    //       'memkind_hbw will not'
    // Note: If HBM is not installed on a system, memkind_hbw_preferred call
    //       would fail. Therefore, we need to check for availability first.
    if (memkind_check_available(MEMKIND_DAX_KMEM) != 0) {
        memkind_kind = MEMKIND_DEFAULT;
    } else {
        memkind_kind = MEMKIND_DAX_KMEM;
    }
}

static void *MemkindMalloc(size_t size)
{
    return memkind_malloc(memkind_kind, size);
}

static void *MemkindCalloc(size_t nmemb, size_t size)
{
    return memkind_calloc(memkind_kind, nmemb, size);
}

static void *MemkindRealloc(void *ptr, size_t size)
{
    return memkind_realloc(memkind_kind, ptr, size);
}

static int MemkindAlign(void **memptr, size_t alignment, size_t size)
{
    return memkind_posix_memalign(memkind_kind, memptr, alignment, size);
}

// memkind_free does not need the exact kind, if kind is 0. Then
// the library can figure out the proper kind itself.
static void MemkindFree(void *ptr)
{
    memkind_free(memkind_kind, ptr);
}

AUTODAX_EXPORT void *malloc(size_t size)
{
    return MemkindMalloc(size);
}

AUTODAX_EXPORT void *calloc(size_t nmemb, size_t size)
{
    return MemkindCalloc(nmemb, size);
}

AUTODAX_EXPORT void *realloc(void *ptr, size_t size)
{
    return MemkindRealloc(ptr, size);
}

AUTODAX_EXPORT int posix_memalign(void **memptr, size_t alignment, size_t size)
{
    return MemkindAlign(memptr, alignment, size);
}

// Warn about deprecated function usage.
AUTODAX_EXPORT void *valloc(size_t size)
{
    void *memptr = 0;
    size_t boundary = sysconf(_SC_PAGESIZE);
    int status = MemkindAlign(&memptr, boundary, size);
    if (status == 0 && memptr != 0)
        return memptr;

    return 0;
}

// Warn about deprecated function usage.
AUTODAX_EXPORT void *memalign(size_t boundary, size_t size)
{
    void *memptr = 0;
    int status = MemkindAlign(&memptr, boundary, size);
    if (status == 0 && memptr != 0)
        return memptr;

    return 0;
}

AUTODAX_EXPORT void free(void *ptr)
{
    return MemkindFree(ptr);
}
