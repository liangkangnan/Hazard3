#include <stdint.h>
#include <stdbool.h>

#include "mach_timer_drv.h"
#include "hazard3_irq.h"


void mach_timer_enable(bool en)
{
    if (en)
        MACH_TIMER->CTRL = 1;
    else
        MACH_TIMER->CTRL = 0;
}

void mach_timer_irq_enable(bool en)
{
    if (en)
        timer_irq_enable(true);
    else
        timer_irq_enable(false);
}

void mach_timer_set_cmp_time(uint64_t cmp)
{
    MACH_TIMER->MTIMECMP = -1u;
    MACH_TIMER->MTIMECMPH = cmp >> 32;
    MACH_TIMER->MTIMECMP = cmp & 0xffffffff;
}

uint64_t mach_timer_get_cmp_time()
{
    uint32_t h0, l, h1;

    do {
        h0 = MACH_TIMER->MTIMECMPH;
        l  = MACH_TIMER->MTIMECMP;
        h1 = MACH_TIMER->MTIMECMPH;
    } while (h0 != h1);

    return l | (uint64_t)h1 << 32;
}

void mach_timer_set_time(uint64_t time)
{
    MACH_TIMER->MTIME = 0;
    MACH_TIMER->MTIMEH = time >> 32;
    MACH_TIMER->MTIME = time & 0xffffffff;
}

uint64_t mach_timer_get_time()
{
    uint32_t h0, l, h1;

    do {
        h0 = MACH_TIMER->MTIMEH;
        l  = MACH_TIMER->MTIME;
        h1 = MACH_TIMER->MTIMEH;
    } while (h0 != h1);

    return l | (uint64_t)h1 << 32;
}
