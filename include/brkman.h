#ifndef _BRKMAN_H_
#define _BRKMAN_H_

#include <stddef.h>
#include <stdbool.h>

#include "attributes.h"
#include "mem.h"

/**
 * @brief Allocate a block of memory.
 *
 * Allocates at least @p size bytes of uninitialized memory.
 *
 * @param size
 *        Number of bytes to allocate.
 *
 * @return
 *        Pointer to the allocated memory on success,
 *        or NULL if the allocation fails (e.g. out of memory).
 *
 * @note
 *        The returned memory is not initialized.
 *        The pointer must be released using brkman_free().
 */
void *brkman_alloc(size_t size);

/**
 * @brief Resize a previously allocated memory block.
 *
 * Changes the size of the memory block pointed to by @p ptr to @p size bytes.
 * The contents are preserved up to the minimum of the old and new sizes.
 *
 * If @p ptr is NULL, this function behaves like brkman_alloc().
 *
 * @param ptr
 *        Pointer to a block previously returned by brkman_alloc(),
 *        brkman_calloc(), or brkman_realloc(), or NULL.
 *
 * @param size
 *        New size in bytes.
 *
 * @return
 *        Pointer to the resized memory block,
 *        or NULL if the reallocation fails.
 *
 * @note
 *        On failure, the original memory block is left unchanged.
 */
void *brkman_realloc(void *_NULLABLE ptr, size_t size);

/**
 * @brief Allocate zero-initialized memory for an array.
 *
 * Allocates memory for an array of @p nmemb elements of @p size bytes each
 * and initializes all bytes in the allocated memory to zero.
 *
 * The function checks for integer overflow in the multiplication
 * @p nmemb * @p size.
 *
 * @param nmemb
 *        Number of elements.
 *
 * @param size
 *        Size of each element in bytes.
 *
 * @return
 *        Pointer to the allocated zero-initialized memory,
 *        or NULL if the allocation fails or an overflow is detected.
 *
 * @note
 *        The returned memory must be released using brkman_free().
 */
void *brkman_calloc(size_t nmemb, size_t size);

/**
 * @brief Free a previously allocated memory block.
 *
 * Releases the memory block pointed to by @p ptr.
 *
 * If @p ptr is NULL, this function has no effect.
 *
 * @param ptr
 *        Pointer to memory previously returned by brkman_alloc(),
 *        brkman_calloc(), or brkman_realloc(), or NULL.
 */
void brkman_free(void *_NULLABLE ptr);

#endif