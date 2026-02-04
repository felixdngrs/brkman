#include "heap.h"

static brkman_heap_t _heap;

ptrdiff_t brkman_cmp_chunks(const brkman_chunk_t* const a,
                            const brkman_chunk_t* const b)
{
    ptrdiff_t chunk_diff = (const char* const) a - (const char* const) b;
    return chunk_diff;
}

brkman_chunk_t* brkman_detach_chunk(brkman_chunk_t* chunk)
{
    UNUSED(chunk);
    return NULL;
}

bool brkman_chunk_make_free(brkman_chunk_t* newchunk)
{
    brkman_chunk_t** free_head = &_heap.free;

    if (!newchunk)
    {
        return false;
    }

    if (0 == _heap.free_chunks_num)
    {
        *free_head = newchunk;
        newchunk->next = NULL;
        newchunk->prev = NULL;
        _heap.free_chunks_num++;
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

            _heap.free_chunks_num++;
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
            _heap.free_chunks_num++;
            return true;
        }

        current_chunk = current_chunk->next;
    }

    return false;
}
void brkman_inc_top_chunk(size_t val)
{
    _heap.top_chunk->size += val;
}
void brkman_dec_top_chunk(size_t val)
{
    _heap.top_chunk->size -= val;
}
void brkman_set_program_break(void* const nbrk)
{
    _heap.program_break = nbrk;
}