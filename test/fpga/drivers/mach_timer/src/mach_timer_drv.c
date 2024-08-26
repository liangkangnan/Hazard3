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
    MACH_TIMER->MTIMECMP = cmp & 0xffffffff;
    MACH_TIMER->MTIMECMPH = (cmp >> 32) & 0xffffffff;
}

uint64_t mach_timer_get_cmp_time()
{
    uint64_t cmp;

    cmp = MACH_TIMER->MTIMECMP;
    cmp += (uint64_t)(MACH_TIMER->MTIMECMPH) << 32;

    return cmp;
}

void mach_timer_set_time(uint64_t time)
{
    MACH_TIMER->MTIME = time & 0xffffffff;
    MACH_TIMER->MTIMEH = (time >> 32) & 0xffffffff;
}

uint64_t mach_timer_get_time()
{
    uint64_t cmp;

    cmp = MACH_TIMER->MTIME;
    cmp += (uint64_t)(MACH_TIMER->MTIMEH) << 32;

    return cmp;
}
