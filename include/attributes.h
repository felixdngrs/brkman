#ifndef _ATTRIBUTES_H_
#define _ATTRIBUTES_H_

#include <stdint.h>

#define UNUSED(param) ((void) (param))

typedef union
{
    long double ld;
    long l;
    long long ll;
    void* p;
    void (*fp)(void);
} brkman_max_align_t;

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

#endif
