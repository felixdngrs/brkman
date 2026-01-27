#include <stddef.h>
#include "mem.h"

typedef struct brkman_chunk_t {
    size_t size;
    brkman_chunk_t *next;
    brkman_chunk_t *prev;
} brkman_chunk_t;

typedef struct brkman_heap_t {
    void *program_break;
    brkman_chunk_t *top_chunk;
    brkman_chunk_t *used;
    brkman_chunk_t *free;
} brkman_heap_t;

brkman_chunk_t *brkman_detach_chunk(brkman_chunk_t *chunk);
bool brkman_insert_chunk(brkman_chunk_t *_NONULL newchunk, brkman_chunk_t *_NONULL lchunk, brkman_chunk_t *_NULLABLE rchunk);
