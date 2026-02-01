#include "heap.h"

static brkman_heap_t _heap;

brkman_chunk_t* brkman_detach_chunk(brkman_chunk_t* chunk)
{
    UNUSED(chunk);
    return NULL;
}

bool brkman_insert_chunk(brkman_chunk_t* newchunk,
                         brkman_chunk_t* lchunk,
                         brkman_chunk_t* rchunk)
{
    UNUSED(newchunk);
    UNUSED(lchunk);
    UNUSED(rchunk);

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