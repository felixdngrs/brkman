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

bool brkman_split_top_chunk(size_t msize)
{
    UNUSED(msize);
    return false;
}