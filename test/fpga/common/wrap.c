#include <stdlib.h>
#include "printf.h"
#include "panic.h"

extern void *__real_malloc(size_t size);
extern void *__real_calloc(size_t count, size_t size);
extern void *__real_realloc(void *mem, size_t size);
extern void __real_free(void *mem);

extern char __StackLimit; /* Set by linker.  */

int __wrap_printf(const char* format, ...)
{
    va_list va;
    va_start(va, format);

    return printf_(format, va);
}

int __wrap_vprintf(const char *format, va_list va)
{
    return vprintf_(format, va);
}

int __wrap_sprintf(char* buffer, const char* format, ...)
{
    va_list va;
    va_start(va, format);

    return sprintf_(buffer, format, va);
}

int __wrap_snprintf_(char* buffer, size_t count, const char* format, ...)
{
    va_list va;
    va_start(va, format);

    return snprintf_(buffer, count, format, va);
}

int __wrap_vsnprintf_(char* buffer, size_t count, const char* format, va_list va)
{
    return vsnprintf_(buffer, count, format, va);
}

int __wrap_puts(const char *s)
{
    return printf_(s);
}

int __wrap_getchar()
{
    return 0;
}

int __wrap_putchar(int c)
{
    return printf_("%c", (char)c);
}

static inline void check_alloc(void *mem, size_t size)
{
    if (!mem || (((char *)mem) + size) > &__StackLimit) {
        panic("Out of memory");
    }
}

void *__wrap_malloc(size_t size)
{
    void *rc = __real_malloc(size);

    check_alloc(rc, size);
    return rc;
}

void *__wrap_calloc(size_t count, size_t size)
{
    void *rc = __real_calloc(count, size);

    check_alloc(rc, count * size);
    return rc;
}

void *__wrap_realloc(void *mem, size_t size)
{
    void *rc = __real_realloc(mem, size);

    check_alloc(rc, size);
    return rc;
}

void __wrap_free(void *mem)
{
    __real_free(mem);
}
