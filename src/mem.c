#include "mem.h"
#include "attributes.h"
#include "heap.h"
#include <stdio.h>

bool heap_shrink(size_t mbytes)
{
    void* shrink_res = sbrk((intptr_t) -mbytes);
    if ((void*) -1 == shrink_res)
    {
        return false;
    }
    brkman_set_program_break(shrink_res);
    return true;
}

#ifdef DEBUG

bool brkman_heap_reset()
{
    const ptrdiff_t heap_size = brkman_get_heap_size();
    bool shrink_status = heap_shrink((size_t) heap_size);
    return shrink_status;
}

#endif

bool heap_extend()
{
    void* old_break = sbrk((intptr_t) BRKMAN_HEAP_EXT_SIZE);
    if ((void*) -1 == old_break)
    {
        return false;
    }
    void* const new_program_break =
        (unsigned char*) old_break + BRKMAN_HEAP_EXT_SIZE;

    /* create a new chunk from the extended heap block */
    brkman_chunk_t* new_chunk =
        (brkman_chunk_t*) ((char*) new_program_break - BRKMAN_HEAP_EXT_SIZE);

    new_chunk->size = BRKMAN_HEAP_EXT_SIZE - BRKMAN_CHUNK_HEADER_SIZE;

    if (!brkman_chunk_mark_free(new_chunk))
    {
        /* in case of an error inserting the new chunk to the free list, we
         * release the allocated memory immediately*/
        void* rollback_res = sbrk((intptr_t) BRKMAN_HEAP_EXT_SIZE);
        if ((void*) -1 == rollback_res)
        {
            /* by now at the latest, we have a fatal error */
            /* TODO: mem.c ; handle fatal error somehow (that applies to the
             * entire project) */
        }
        return false;
    }

    /* TODO: mem.c ; merge chunks in heap_extend() (all chunks or at least the
     * last two) */

    brkman_set_program_break(new_program_break);

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

    ret_chunk = brkman_search_free(membytes);
    return ret_chunk;
}
