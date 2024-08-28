#include <stdint.h>
#include "uart_drv.h"
#include "xprintf.h"
#include "hazard3_csr.h"

void __attribute__((interrupt)) isr_riscv_machine_exception()
{
    uart_init(115200);
    xdev_out(uart_putc);

    xprintf("!!! handle exception !!!\n");
    xprintf("exception, mcause = %u\n", read_csr(mcause));

    uint32_t mepc = read_csr(mepc);

    xprintf("exception pc: 0x%08x\n", mepc);
    if ((*(uint16_t*)mepc & 0x3) == 0x3) {
        xprintf("exception instr: %04x%04x\n", *(uint16_t*)(mepc + 2), *(uint16_t*)mepc);
    } else {
        xprintf("exception instr: %04x\n", *(uint16_t*)mepc);
    }

    while (1);
}

extern void runtime_init_per_core_h3_irq_registers(void);

void runtime_init()
{
    runtime_init_per_core_h3_irq_registers();
}
