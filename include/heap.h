#ifndef _HEAP_H_
#define _HEAP_H_

#include <stddef.h>
#include <stdbool.h>
#include "attributes.h"

typedef struct brkman_chunk_t {
    size_t size;
    struct brkman_chunk_t *next;
    struct brkman_chunk_t *prev;
} brkman_chunk_t;

typedef struct brkman_heap_t {
    void *program_break;
    brkman_chunk_t *top_chunk;
    brkman_chunk_t *used;
    brkman_chunk_t *free;
} brkman_heap_t;

brkman_chunk_t *brkman_detach_chunk(brkman_chunk_t *chunk) __attribute__((warn_unused_result));
bool brkman_insert_chunk(brkman_chunk_t * newchunk, brkman_chunk_t * lchunk, brkman_chunk_t * rchunk) __attribute__((warn_unused_result));
void brkman_inc_top_chunk(size_t val);
void brkman_dec_top_chunk(size_t val);
void brkman_set_program_break(const void * const nbrk);

#endif