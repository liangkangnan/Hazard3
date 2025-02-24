#ifndef _PERIRESET_DRV_H_
#define _PERIRESET_DRV_H_

#define PERIRESET_BASE_ADDR  (0x40002000)

#define TIMER_RESET_BIT      0x1
#define UART_RESET_BIT       0x2
#define XIP_RESET_BIT        0x4
#define PIO0_RESET_BIT       0x8

typedef struct {
    volatile uint32_t ctrl;
} perireset_t;

#define perireset_hw ((perireset_t *)PERIRESET_BASE_ADDR)

static inline void peri_reset(uint32_t bits) {
    perireset_hw->ctrl |= bits;
    while (perireset_hw->ctrl & bits);
}

#endif
