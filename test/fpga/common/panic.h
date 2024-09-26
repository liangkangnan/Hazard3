#ifndef _PANIC_H_
#define _PANIC_H_

#include "printf.h"

static inline void panic(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    printf("!!! panic !!!\n");
    printf(fmt, va);

    asm("1: j 1b\n");
}

#endif
