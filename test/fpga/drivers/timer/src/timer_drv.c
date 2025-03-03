#include "timer_drv.h"


void timer_enable(timer_t *t, bool en)
{
    if (en)
        t->CSR |= 1 << TIMER_CSR_EN_LSB;
    else
        t->CSR &= ~(1 << TIMER_CSR_EN_LSB);
}

void timer_int_enable(timer_t *t, bool en)
{
    if (en)
        t->CSR |= 1 << TIMER_CSR_INT_EN_LSB;
    else
        t->CSR &= ~(1 << TIMER_CSR_INT_EN_LSB);
}

void timer_set_mode(timer_t *t, timer_mode_e mode)
{
    t->CSR = (t->CSR & (~TIMER_CSR_MODE_MASK)) | (mode << TIMER_CSR_MODE_LSB);
}

void timer_set_div(timer_t *t, uint16_t div)
{
    t->CSR = (t->CSR & (~TIMER_CSR_DIV_MASK)) | (div << TIMER_CSR_DIV_LSB);
}

void timer_clear_int_pending(timer_t *t)
{
    t->PENDING |= 1 << TIMER_INT_PENDING_PENDING_LSB;
}

uint8_t timer_get_int_pending(timer_t *t)
{
    return (!!(t->PENDING & TIMER_INT_PENDING_PENDING_MASK));
}

void timer_set_expire_count(timer_t *t, uint32_t count)
{
    t->EXPIRE_COUNT = count;
}

uint32_t timer_get_current_count(timer_t *t)
{
    return t->CURRENT_COUNT;
}
