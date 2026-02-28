#ifndef _HEAP_H_
#define _HEAP_H_

#include "attributes.h"
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

typedef struct brkman_chunk_t
{
    size_t size;
    struct brkman_chunk_t* next;
    struct brkman_chunk_t* prev;
} brkman_chunk_t;

typedef struct brkman_heap_t
{
    void* initial_break;
    size_t free_mem_bytes;
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

bool brkman_claim_chunk(brkman_chunk_t* chunk)
    __attribute__((warn_unused_result));

/**
 * @brief Gets the current size of the heap in bytes.
 *
 * This function calculates the difference between the current program break
 * and the initial break address, effectively returning the total heap size
 * allocated by the memory manager.
 *
 * @return ptrdiff_t The size of the heap in bytes.
 */
ptrdiff_t brkman_get_heap_size();

/**
 * @brief Returns the initial program break of the heap.
 *
 * This function retrieves the initial program break using sbrk(0)
 * and stores it internally on first invocation. Subsequent calls
 * return the cached value.
 *
 * @return A pointer to the initial program break.
 * @retval NULL If the program break could not be determined.
 */
char* brkman_get_initial_break();

/**
 * @brief Returns the user-accessible memory region of a heap chunk.
 *
 * This function extracts the usable payload area from a given heap chunk.
 * Each chunk contains an internal header (metadata) at the beginning
 * that must not be exposed to the user. The function therefore returns
 * a pointer to the memory region immediately following the chunk header.
 *
 * Internally, this is implemented by adding the size of the chunk header
 * to the base address of the chunk.
 *
 * @param chunk Pointer to the heap chunk structure.
 *
 * @return Pointer to the beginning of the user-accessible memory area
 *         (i.e., the payload region). Returns NULL if @p chunk is NULL.
 *
 * @note The returned pointer must be treated as opaque user memory.
 *       Accessing memory before this pointer may corrupt allocator metadata.
 */
void* brkman_heap_payload_of(brkman_chunk_t* chunk);

#ifdef DEBUG
/**
 * @brief Returns the heap chunk structure associated with a user payload
 * pointer.
 *
 * This function reconstructs the internal heap chunk metadata structure
 * from a pointer to the user-accessible memory region (payload).
 * Since the allocator stores a chunk header directly before the payload,
 * the function computes the address of the chunk by subtracting the
 * size of the chunk header from the given payload pointer.
 *
 * Internally, this is implemented by subtracting sizeof(brkman_chunk_t)
 * from the payload address.
 *
 * @param payload Pointer to the beginning of the user-accessible memory
 *                region (as previously returned by the allocator).
 *
 * @return Pointer to the corresponding heap chunk structure.
 *         Returns NULL if @p payload is NULL.
 *
 * @note The payload pointer must originate from this allocator.
 *       Passing arbitrary or invalid pointers results in undefined behavior.
 *
 * @warning Accessing or modifying the returned chunk directly may
 *          corrupt allocator metadata and should only be done internally.
 */
const brkman_chunk_t* brkman_heap_header_of(const void* payload);

/**
 * @brief Returns a read-only reference to the global heap.
 *
 * This function provides access to the internal global heap structure
 * used by the allocator. It is intended for debugging and inspection
 * purposes only, allowing external code (e.g., tests or diagnostics)
 * to read allocator metadata without modifying it.
 *
 * @return pointer to the global heap structure.
 *
 * @note This function is only available in DEBUG mode.
 *       It must not be exposed in release builds, as external access
 *       to allocator metadata could compromise heap integrity.
 */
const brkman_heap_t* brkman_heap_ref(void);

/**
 * @brief Sets the internal value for free memory bytes to zero.
 *
 * This function sets the internal value for free memory bytes to zero.
 *
 * @note This function is only available in DEBUG mode, as the heap manages his
 * metadate by itself in release mode.
 */
void brkman_heap_reset_size(void);
#endif

#endif
