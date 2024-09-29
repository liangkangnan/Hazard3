#ifndef _DELAY_H_
#define _DELAY_H_

#include "hazard3_csr.h"

static inline uint64_t riscv_get_cycle_time()
{
    // Read procedure from RISC-V ISA manual to avoid being off by 2**32 on
    // low half rollover -- note this loop generally executes only once, and
    // should never execute more than twice:
    uint32_t h0, l, h1;
    do {
        h0 = read_csr(mcycleh);
        l  = read_csr(mcycle);
        h1 = read_csr(mcycleh);
    } while (h0 != h1);
    return l | (uint64_t)h1 << 32;
}

void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

#endif
