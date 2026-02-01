#ifndef _ATTRIBUTES_H_
#define _ATTRIBUTES_H_

#define UNUSED(param) ((void) (param))

typedef union
{
    long double ld;
    long l;
    long long ll;
    void* p;
    void (*fp)(void);
} brkman_max_align_t;

#endif