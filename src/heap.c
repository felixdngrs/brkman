#include "heap.h"
#include <stdio.h>

static brkman_heap_t _heap = {.initial_break = NULL,
                              .program_break = NULL,
                              .free_mem_bytes = 0,
                              .free = NULL};

ptrdiff_t brkman_cmp_chunks(const brkman_chunk_t* const a,
                            const brkman_chunk_t* const b)
{
    ptrdiff_t chunk_diff = (const char* const) a - (const char* const) b;
    return chunk_diff;
}

size_t brkman_get_free_bytes()
{
    return _heap.free_mem_bytes;
}

bool brkman_reclaim_chunk(brkman_chunk_t* chunk)
{
    if (!chunk)
    {
        return false;
    }
    if (chunk->next && chunk->prev)
    {
        chunk->prev->next = chunk->next;
        chunk->next->prev = chunk->prev;
        chunk->next = NULL;
        chunk->prev = NULL;
        _heap.free_mem_bytes -= chunk->size;
    }
    else if (chunk->next && !chunk->prev)
    {
        _heap.free = chunk->next;
        _heap.free->prev = NULL;
        chunk->next = NULL;
        _heap.free_mem_bytes -= chunk->size;
    }
    else if (!chunk->next && chunk->prev)
    {
        chunk->prev->next = NULL;
        chunk->prev = NULL;
        _heap.free_mem_bytes -= chunk->size;
    }
    else if (!chunk->next && !chunk->prev)
    {
        _heap.free_mem_bytes = 0;
        _heap.free = NULL;
    }
    return true;
}

bool brkman_chunk_mark_free(brkman_chunk_t* newchunk)
{
    brkman_chunk_t** free_head = &_heap.free;

    if (!newchunk)
    {
        return false;
    }

    if (0 == _heap.free_mem_bytes)
    {
        *free_head = newchunk;
        newchunk->next = NULL;
        newchunk->prev = NULL;
        _heap.free_mem_bytes += newchunk->size;
        return true;
    }

    brkman_chunk_t* current_chunk = *free_head;

    while (current_chunk)
    {
        ptrdiff_t diff = brkman_cmp_chunks(current_chunk, newchunk);

        bool newchunk_has_smaller_addr = (diff > 0);
        bool newchunk_is_current = !diff;

        if (newchunk_has_smaller_addr)
        {
            if (current_chunk != *free_head)
            {
                brkman_chunk_t* tmp_chunk = current_chunk->prev;
                current_chunk->prev->next = newchunk;
                current_chunk->prev = newchunk;
                newchunk->prev = tmp_chunk;
            }
            newchunk->next = current_chunk;

            if (current_chunk == *free_head)
            {
                newchunk->prev = NULL;
                *free_head = newchunk;
            }

            _heap.free_mem_bytes += newchunk->size;
            return true;
        }

        if (newchunk_is_current)
        {
            /* newchunk is already included in free */
            return false;
        }

        if (current_chunk->next == NULL)
        {
            /* We reached the last chunk, a comparison isn"t possible
             * anymore. */
            current_chunk->next = newchunk;
            newchunk->prev = current_chunk;
            newchunk->next = NULL;
            _heap.free_mem_bytes += newchunk->size;
            return true;
        }

        current_chunk = current_chunk->next;
    }

    return false;
}

brkman_chunk_t* brkman_search_free(size_t minsize)
{
    brkman_chunk_t* next_fit = NULL;
    for (brkman_chunk_t* current = _heap.free; current != NULL;
         current = current->next)
    {
        if (current->size == minsize)
        {
            next_fit = current;
            break;
        }
        if (current->size > minsize)
        {
            bool current_has_better_fit =
                next_fit && (next_fit->size > current->size);
            if (!next_fit || current_has_better_fit)
            {
                next_fit = current;
            }
        }
    }

    /* after requesting the next available chunk we try to split it, so it fits
     * better */
    brkman_chunk_t* ret_chunk = brkman_split_chunk(next_fit, minsize);

    return ret_chunk;
}

void brkman_set_program_break(void* const nbrk)
{
    _heap.program_break = nbrk;
}

brkman_chunk_t* brkman_split_chunk(brkman_chunk_t* chunk, size_t msize)
{
    if (!chunk)
    {
        return NULL;
    }

    /* Is the chunk too small for the request? */
    if (msize > chunk->size)
    {
        return chunk;
    }

    size_t remaining_size = chunk->size - msize;

    /* If the remaining size is too small to be useful, return the original
     * chunk. */
    if (remaining_size < BRKMAN_MIN_CHUNK_SIZE)
    {
        return chunk;
    }

    /* Calculate where to split the chunk */
    char* chunk_end = (char*) chunk + (chunk->size);
    char* break_chunk_at = chunk_end - msize;
    brkman_chunk_t* new_chunk = (brkman_chunk_t*) break_chunk_at;

    /* Adjust the sizes of the chunks */
    chunk->size -= msize;
    new_chunk->size = msize;

    /* Update the free list after splitting the chunk */
    if (!brkman_chunk_mark_free(new_chunk))
    {
        /* in this case, we successfully splitted one chunk but weren't able to
         * update the free list somehow. We cannot return the new chunk here
         * because it's not marked as free. We have a strange and inconsistent
         * state here. We'll simply return NULL for now. Actually we have to
         * deal with this error more cleanly. */
        /* TODO: heap.c ; handle brkman_chunk_mark_free error in
         * brkman_split_chunk */
    }

    return new_chunk;
}

brkman_chunk_t* brkman_merge_chunks(brkman_chunk_t* restrict chunk1,
                                    brkman_chunk_t* restrict chunk2)
{
    if (!chunk1 || !chunk2)
    {
        return NULL;
    }

    /* verify that the chunks are adjacent in memory */
    char* addr_after_ch1 = (char*) chunk1 + chunk1->size;
    if (addr_after_ch1 != (char*) chunk2)
    {
        return NULL;
    }

    chunk1->next = chunk2->next;

    if (chunk1->next)
    {
        chunk1->next->prev = chunk1;
    }

    chunk1->size += chunk2->size;
    return chunk1;
}

ptrdiff_t brkman_get_heap_size()
{
    ptrdiff_t diff = (char*) _heap.program_break - (char*) _heap.initial_break;
    return diff;
}
