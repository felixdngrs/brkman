#include "brkman.h"

void* brkman_alloc(size_t size)
{
    void* ret_mem = brkman_mem_alloc(size);
    return ret_mem;
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
    brkman_mem_free(ptr);
}
