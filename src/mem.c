#include "mem.h"
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

brkman_chunk_t* brkman_split_chunk(brkman_chunk_t* chunk, size_t msize)
{
    if (!chunk)
    {
        return NULL;
    }

    /* is the chunk too small for the request? */
    if (msize > chunk->size)
    {
        return NULL;
    }

    size_t remaining_size = chunk->size - msize;

    /* that is ok, we can return 'chunk' and do nothing. The caller gets a bit
     * more memory as requested. Otherwise the remaining chunk would be too
     * small to be used again.  */
    if (remaining_size < BRKMAN_MIN_CHUNK_SIZE)
    {
        return chunk;
    }

    char* chunk_end = (char*) chunk + (chunk->size);
    char* break_chunk_at = chunk_end - msize;
    brkman_chunk_t* new_chunk = (brkman_chunk_t*) break_chunk_at;

    chunk->size -= msize;
    new_chunk->size = msize;

    /* as we can only split free chunks, we have to update the free list after
     * splitting the chunk. */
    brkman_chunk_make_free(new_chunk);

    return new_chunk;
}
