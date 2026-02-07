#ifndef _MEM_H_
#define _MEM_H_

#include "attributes.h"
#include "heap.h"
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#define BRKMAN_ALIGNOF(type)                                                   \
    (offsetof(                                                                 \
        struct {                                                               \
            uint8_t c;                                                         \
            type m;                                                            \
        },                                                                     \
        m))
#define BRKMAN_HEAP_EXT_SIZE (64 * 1024)
#define BRKMAN_CHUNK_HEADER_SIZE (sizeof(brkman_chunk_t))
#define BRKMAN_CHUNK_MIN_PAYLOAD (sizeof(uint8_t))
#define BRKMAN_ALIGN_UP(size)                                                  \
    (((size) + BRKMAN_ALIGNOF(brkman_max_align_t) - 1) &                       \
     ~(BRKMAN_ALIGNOF(brkman_max_align_t)))
#define BRKMAN_IS_ALIGNED(size) (BRKMAN_ALIGN_UP((size)) == (size))
#define BRKMAN_MIN_CHUNK_UNALIGNED                                             \
    (BRKMAN_CHUNK_HEADER_SIZE + BRKMAN_CHUNK_MIN_PAYLOAD)
#define BRKMAN_MIN_CHUNK_SIZE (BRKMAN_ALIGN_UP(BRKMAN_MIN_CHUNK_UNALIGNED))

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

#endif
