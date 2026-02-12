#include "mem.h"
#include "attributes.h"
#include "heap.h"

bool heap_extend()
{
    void* old_break = sbrk((intptr_t) BRKMAN_HEAP_EXT_SIZE);
    if ((void*) -1 == old_break)
    {
        return false;
    }
    void* const new_program_break =
        (unsigned char*) old_break + BRKMAN_HEAP_EXT_SIZE;
    brkman_set_program_break(new_program_break);
    brkman_inc_top_chunk(BRKMAN_HEAP_EXT_SIZE);

    return true;
}

brkman_chunk_t* brkman_mem_alloc(size_t membytes)
{
    brkman_chunk_t* ret_chunk = NULL;

    if (membytes < BRKMAN_MIN_CHUNK_SIZE)
    {
        membytes = BRKMAN_MIN_CHUNK_SIZE;
    }
    else if (!BRKMAN_IS_ALIGNED(membytes))
    {
        membytes = BRKMAN_ALIGN_UP(membytes);
    }
    else
    {
        /* this isn"t considered an error. We simply return a freeable
         * NULL-pointer, as the caller asked for no memory. */
        return NULL;
    }

    size_t free_bytes = brkman_get_free_bytes();

    if (free_bytes < membytes)
    {
        /* No sufficient memory left - enlarge heap */
        if (!heap_extend())
        {
            return NULL;
        }
    }
    else
    {
        /* sufficient memory left - ask for an appropriate chunk */
        ret_chunk = brkman_search_free(membytes);
    }

    return ret_chunk;
}
