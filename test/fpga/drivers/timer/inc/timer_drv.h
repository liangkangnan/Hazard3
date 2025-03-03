#ifndef _TIMER_DRV_H_
#define _TIMER_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#include "timer_regs.h"

#define TIMER0_BASE_ADDR  (0x40003000)

typedef enum {
    TIMER_MODE_ONESHOT = 0,
    TIMER_MODE_RELOAD = 1
} timer_mode_e;

typedef struct {
    volatile uint32_t CSR;
    volatile uint32_t PENDING;
    volatile uint32_t EXPIRE_COUNT;
    volatile uint32_t CURRENT_COUNT;
} timer_t;

#define TIMER0 ((timer_t *)TIMER0_BASE_ADDR)

void timer_enable(timer_t *t, bool en);
void timer_int_enable(timer_t *t, bool en);
void timer_set_mode(timer_t *t, timer_mode_e mode);
void timer_set_div(timer_t *t, uint16_t div);
void timer_clear_int_pending(timer_t *t);
uint8_t timer_get_int_pending(timer_t *t);
void timer_set_expire_count(timer_t *t, uint32_t count);
uint32_t timer_get_current_count(timer_t *t);

#endif
