#include <stdint.h>
#include "hazard3_csr.h"

#define MHZ(a) (a##000000)

static volatile uint32_t core_clk_hz;

extern void runtime_init_per_core_h3_irq_registers(void);

void clock_init()
{
    core_clk_hz = MHZ(12);
}

uint32_t get_core_clock_hz()
{
    return core_clk_hz;
}

uint32_t get_uart_clock_hz()
{
    return core_clk_hz;
}

void runtime_init()
{
    clock_init();

    runtime_init_per_core_h3_irq_registers();
}
