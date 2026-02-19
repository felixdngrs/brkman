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

/**
 * @brief Shrinks the heap by the specified number of bytes.
 *
 * Attempts to reduce the program's heap size by moving the program break
 * downwards by the given number of bytes. This function uses the system
 * call `sbrk()` to adjust the heap size.
 *
 * @param mbytes The number of bytes by which to shrink the heap.
 * @return true  If the heap was successfully shrunk.
 * @return false If the operation failed (e.g., not enough memory to shrink,
 *               or the system call returned an error).
 */
bool heap_shrink(size_t mbytes) __attribute__((warn_unused_result));

#ifdef DEBUG

bool brkman_heap_reset();
#endif

void* brkman_mem_alloc(size_t membytes) __attribute__((warn_unused_result));

#endif
