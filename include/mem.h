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

/**
 * @brief Splits a free memory chunk into two parts if possible.
 *
 * This function attempts to split the given chunk so that a new chunk of size
 * `msize` can be returned. The split is performed from the end of the chunk.
 * If the remaining size would be too small to be usable (less than
 * BRKMAN_MIN_CHUNK_SIZE), the original chunk is returned unmodified.
 *
 * @param chunk Pointer to the chunk to split. Must not be NULL.
 * @param msize Size in bytes of the chunk to allocate. Must include the header
 *              and be aligned according to BRKMAN_ALIGN_UP.
 *
 * @return Pointer to the newly created chunk of size `msize`, or
 *         the original chunk if splitting is not possible.
 *
 * @note Both the original and new chunks must be free. The function updates
 *       the free list for the new chunk by calling brkman_chunk_make_free().
 * @note The function assumes that all chunk sizes are already properly aligned.
 */
brkman_chunk_t* brkman_split_chunk(brkman_chunk_t* chunk, size_t msize)
    __attribute__((warn_unused_result));

/**
 * @brief Merges two adjacent free chunks into a single larger chunk.
 *
 * This function merges @p chunk1 and @p chunk2 into one contiguous chunk.
 * Both chunks must be free and located directly next to each other in memory.
 * The resulting chunk starts at the address of @p chunk1 and has a size equal
 * to the sum of both chunks (including their headers).
 *
 * @warning
 * The function does NOT verify that the chunks are adjacent in memory.
 * The caller is fully responsible for ensuring that @p chunk1 and @p chunk2
 * are physically consecutive and compatible for merging. Passing non-adjacent
 * chunks results in undefined behavior.
 *
 * The function does not update free lists or other allocator metadata.
 * Callers must handle all necessary bookkeeping.
 *
 * @param chunk1 Pointer to the first chunk. Must be non-NULL and precede
 *               @p chunk2 in memory.
 * @param chunk2 Pointer to the second chunk. Must be non-NULL and immediately
 *               follow @p chunk1 in memory.
 *
 * @return Pointer to the merged chunk (same address as @p chunk1).
 *
 * @pre
 * - @p chunk1 and @p chunk2 are valid chunk pointers.
 * - Both chunks are free (not allocated).
 * - @p chunk1 and @p chunk2 are consecutive in memory (caller-guaranteed).
 *
 * @post
 * - The returned chunk represents the combined memory of both input chunks.
 * - @p chunk2 must not be accessed after the call.
 */
brkman_chunk_t* brkman_merge_chunks(brkman_chunk_t* restrict chunk1,
                                    brkman_chunk_t* restrict chunk2)
    __attribute__((warn_unused_result));
#endif
