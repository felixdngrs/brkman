#include "mem.h"
#include "attributes.h"
#include "heap.h"
#include <stdio.h>
#include <sys/resource.h>

bool heap_shrink(size_t mbytes)
{
    void* shrink_res = sbrk((intptr_t) -mbytes);
    if ((void*) -1 == shrink_res)
    {
        return false;
    }
    return true;
}

#ifdef DEBUG

bool brkman_heap_reset()
{
    const ptrdiff_t heap_size = brkman_get_heap_size();
    bool shrink_status = heap_shrink((size_t) heap_size);
    brkman_heap_reset_size();
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
    char* const new_program_break = sbrk(0);

    /* create a new chunk from the extended heap block */
    brkman_chunk_t* new_chunk =
        (brkman_chunk_t*) (new_program_break - BRKMAN_HEAP_EXT_SIZE);

    new_chunk->size = BRKMAN_HEAP_EXT_SIZE;

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

    /* here we set the initial break, if not happened yet */
    (void) brkman_get_initial_break();

    return true;
}

void* brkman_mem_alloc(size_t membytes)
{
    brkman_chunk_t* ret_chunk = NULL;

    /* we always need to add the header size (because the chunk always has a
     * header) */
    if (__builtin_add_overflow(membytes, BRKMAN_CHUNK_HEADER_SIZE, &membytes))
    {
        /* overflow */
        return NULL;
    }

    struct rlimit rlim;

    if (getrlimit(RLIMIT_DATA, &rlim) < 0)
    {
        return NULL;
    }

    if (membytes >= rlim.rlim_cur)
    {
        return NULL;
    }

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

    if (!brkman_claim_chunk(ret_chunk))
    {
        return NULL;
    }

    void* ret_mem = brkman_heap_payload_of(ret_chunk);

    return ret_mem;
}

void brkman_mem_free(void* payload)
{
    if (NULL != payload)
    {
        brkman_chunk_t* chunk =
            (brkman_chunk_t*) ((char*) payload - BRKMAN_CHUNK_HEADER_SIZE);
        bool free_status = brkman_chunk_mark_free(chunk);
        UNUSED(free_status);
    }
    /* noop */
}
