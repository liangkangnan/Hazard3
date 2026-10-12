#include <stdint.h>
#include "hazard3_csr.h"
#include "perireset_drv.h"

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

#ifndef NO_PERI_RESET
    /* 只有 a7-lite 的 SoC 里有 peri_reset 这个外设(cmod-a7 的 SoC 没有):
       在 cmod-a7 上碰它会读到总线错误 -> 直接进异常处理 -> 到不了 main。
       目标是 cmod-a7 的程序链接时加 -DNO_PERI_RESET 跳过这一步
       (Makefile 里写 NO_PERI_RESET := 1 即可, 见 common.mk)。 */
    peri_reset(PIO_RESET_BIT);
#endif

    runtime_init_per_core_h3_irq_registers();
}
