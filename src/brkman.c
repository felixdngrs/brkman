#include "brkman.h"

void* brkman_alloc(size_t size)
{
    /* TEST! we will return the full chunk for now (with header) */
    brkman_chunk_t* ret_chunk = brkman_mem_alloc(size);
    return ret_chunk;
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
