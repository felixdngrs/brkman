#include "brkman.h"

void* brkman_alloc(size_t size)
{
    // the allocated memory block has to be aligned properly
    if (!BRKMAN_IS_ALIGNED(size))
    {
        size = BRKMAN_ALIGN_UP(size);
    }

    // TODO: alloc logic here

    return NULL;
}

void* brkman_realloc(void* ptr, size_t size)
{
    UNUSED(ptr);
    UNUSED(size);
    return NULL;
}

void* brkman_calloc(size_t nmemb, size_t size)
{
    UNUSED(nmemb);
    UNUSED(size);
    return NULL;
}

void brkman_free(void* ptr)
{
    UNUSED(ptr);
}