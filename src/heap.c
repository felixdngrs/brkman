#include "heap.h"

static brkman_heap_t _heap;

ptrdiff_t brkman_cmp_chunks(const brkman_chunk_t* const a,
                            const brkman_chunk_t* const b)
{
    ptrdiff_t chunk_diff = (const char* const) a - (const char* const) b;
    return chunk_diff;
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
        _heap.free_chunks_num--;
    }
    else if (chunk->next && !chunk->prev)
    {
        _heap.free = chunk->next;
        _heap.free->prev = NULL;
        chunk->next = NULL;
        _heap.free_chunks_num--;
    }
    else if (!chunk->next && chunk->prev)
    {
        chunk->prev->next = NULL;
        chunk->prev = NULL;
        _heap.free_chunks_num--;
    }
    else if (!chunk->next && !chunk->prev)
    {
        _heap.free_chunks_num = 0;
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
    return next_fit;
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
