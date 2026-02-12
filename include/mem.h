#ifndef _MEM_H_
#define _MEM_H_

#include "attributes.h"
#include "heap.h"
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

/**
 * @brief Extends the heap by a predefined increment.
 *
 * This function increases the top chunk of the heap to provide additional
 * memory space for future allocations. The expansion always occurs by a
 * fixed, globally defined value.
 *
 * @param heap A pointer to the heap structure to be extended.
 * Must not be NULL.
 *
 * @return true  If the extension was successful and the top_chunk was enlarged.
 * @return false If the extension failed (e.g., due to the operating system
 * or lack of memory).
 */
bool heap_extend(void) __attribute__((warn_unused_result));

brkman_chunk_t* brkman_mem_alloc(size_t membytes)
    __attribute__((warn_unused_result));

#endif
