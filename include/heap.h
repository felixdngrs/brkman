#ifndef _HEAP_H_
#define _HEAP_H_

#include "attributes.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct brkman_chunk_t
{
    size_t size;
    struct brkman_chunk_t* next;
    struct brkman_chunk_t* prev;
} brkman_chunk_t;

typedef struct brkman_heap_t
{
    void* program_break;
    size_t free_mem_bytes;
    brkman_chunk_t* top_chunk;
    brkman_chunk_t* free;
} brkman_heap_t;

ptrdiff_t brkman_cmp_chunks(const brkman_chunk_t* const a,
                            const brkman_chunk_t* const b)
    __attribute__((warn_unused_result));
brkman_chunk_t* brkman_detach_chunk(brkman_chunk_t* chunk)
    __attribute__((warn_unused_result));
bool brkman_chunk_mark_free(brkman_chunk_t* newchunk)
    __attribute__((warn_unused_result));
/**
 * @brief Increases the size of the current top heap chunk.
 *
 * This function adds the given value to the size of the heap's top chunk.
 * It assumes that the top chunk is valid and that the resulting size
 * remains within allowed bounds.
 *
 * @param val Number of bytes to add to the top chunk size.
 */
void brkman_inc_top_chunk(size_t val);
/**
 * @brief Decreases the size of the current top heap chunk.
 *
 * This function subtracts the given value from the size of the heap's
 * top chunk. The caller is responsible for ensuring that the size does
 * not underflow.
 *
 * @param val Number of bytes to subtract from the top chunk size.
 */
void brkman_dec_top_chunk(size_t val);

/**
 * @brief Sets the program break pointer used by the heap manager.
 *
 * This function updates the internal program break value, which represents
 * the current end of the heap. It does not perform any memory allocation
 * or validation; it simply assigns the provided address.
 *
 * @param nbrk Pointer to the new program break address.
 */
void brkman_set_program_break(void* const nbrk);

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
 *       the free list for the new chunk by calling brkman_chunk_mark_free().
 * @note The function assumes that all chunk sizes are already properly aligned.
 */
brkman_chunk_t* brkman_split_chunk(brkman_chunk_t* chunk, size_t msize)
    __attribute__((warn_unused_result));

/**
 * @brief Merge two adjacent memory chunks into one.
 *
 * This function merges two contiguous memory chunks by extending the first
 * chunk (chunk1) to include the memory occupied by the second chunk (chunk2).
 * After the merge, chunk2 is removed from the chunk list and must no longer
 * be accessed.
 *
 * The function only succeeds if both chunks are valid and directly adjacent
 * in memory.
 *
 * @param chunk1 Pointer to the first chunk, which will absorb the second chunk.
 * @param chunk2 Pointer to the second chunk, which will be merged into the
 * first.
 *
 * @return Pointer to the merged chunk (chunk1) on success, or NULL if
 *         either pointer is NULL or the chunks are not adjacent in memory.
 *
 * @note The chunks must be contiguous in memory (chunk2 must immediately
 *       follow chunk1).
 * @note After a successful merge, chunk2 is no longer part of the chunk list.
 */
brkman_chunk_t* brkman_merge_chunks(brkman_chunk_t* restrict chunk1,
                                    brkman_chunk_t* restrict chunk2);

/**
 * @brief Returns the total free heap memory in bytes.
 *
 * This function reports the raw amount of free memory currently available
 * in the heap. The returned value includes all allocator metadata, such as
 * chunk headers, and therefore does not represent the amount of memory
 * that can necessarily be allocated by the user.
 *
 * @return Total free heap memory in bytes, including allocator overhead.
 */
size_t brkman_get_free_bytes();

/**
 * @brief Searches the free list for a chunk of memory that can satisfy a
 * request.
 *
 * This function implements a "next-fit" search strategy to find a free
 * memory chunk that is at least as large as the requested size. If an exact
 * match is found, it is returned immediately. Otherwise, the smallest
 * suitable chunk is selected.
 *
 * @param minsize The minimum size (in bytes) of the chunk to search for.
 * @return brkman_chunk_t* A pointer to the first suitable free chunk, or
 * NULL if none is found.
 */
brkman_chunk_t* brkman_search_free(size_t minsize)
    __attribute__((warn_unused_result));

#endif
