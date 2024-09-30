#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "hazard3_csr.h"

static inline void riscv_sp_over_detect_enable()
{
    set_csr(hazard3_csr_mspbottom, 0x1);
}

static inline void riscv_sp_over_detect_disable()
{
    clear_csr(hazard3_csr_mspbottom, 0x1);
}

static inline void riscv_set_sp_bottom(uint32_t addr)
{
    addr = addr & (~0x3);
    uint32_t sp_bottom = read_csr(hazard3_csr_mspbottom);
    addr |= sp_bottom & 0x3;
    write_csr(hazard3_csr_mspbottom, addr);
}

static inline void riscv_trigger_soft_irq()
{
    write_csr(hazard3_csr_mswint, 0x1);
}

void clock_init();
uint32_t get_core_clock_hz();
uint32_t get_uart_clock_hz();

#endif
