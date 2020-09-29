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

static memkind_t kind;

#define KMEM_DAX_EXPORT __attribute__((visibility("default")))
#define KMEM_DAX_INIT __attribute__((constructor))

static void KMEM_DAX_INIT kmem_init(void){
   const char *log_str = getenv("KMEM_PATH");
   if (log_str)
       fprintf(stdout, "\n KMEM_PAHT %s", log_str);
    kind = MEMKIND_DEFAULT;
    fprintf(stdout, "\n kmem_init");
}

static void *kmem_malloc(size_t size)
{
    fprintf(stdout, "\n enter malloc size: %zu", size);
    return memkind_malloc(kind, size);
}

KMEM_DAX_EXPORT void *malloc(size_t size)
{
    return kmem_malloc(size);
}

KMEM_DAX_EXPORT void *calloc(size_t nmemb, size_t size)
{
    fprintf(stdout, "\n enter calloc nmemb: %zu size: %zu", nmemb, size);
    return memkind_calloc(kind, nmemb, size);
}

KMEM_DAX_EXPORT void *realloc(void *ptr, size_t size)
{
    fprintf(stdout, "\n enter realloc size: %zu", size);
    return memkind_realloc(kind, ptr, size);
}

KMEM_DAX_EXPORT int posix_memalign(void **memptr, size_t alignment, size_t size)
{
    fprintf(stdout, "\n enter posix_memalign size: %zu", size);
    return memkind_posix_memalign(kind, memptr, alignment, size);
}

KMEM_DAX_EXPORT void free(void *ptr)
{
    fprintf(stdout, "\n enter free");
    memkind_free(kind, ptr);
}