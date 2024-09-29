#include <stdint.h>
#include "delay.h"
#include "system.h"

void delay_ms(uint32_t ms)
{
    uint64_t start;
    uint32_t time;

    time = get_core_clock_hz() * ms / 1000;
    start = riscv_get_cycle_time();

    while (riscv_get_cycle_time() < (start + time));
}

void delay_us(uint32_t us)
{
    uint64_t start;
    uint32_t time;

    time = get_core_clock_hz() * us / 1000000;
    start = riscv_get_cycle_time();

    while (riscv_get_cycle_time() < (start + time));
}
