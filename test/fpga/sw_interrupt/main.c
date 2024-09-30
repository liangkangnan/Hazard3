#include <stdint.h>

#include "uart_drv.h"
#include "printf.h"
#include "system.h"
#include "hazard3_irq.h"
#include "delay.h"


int main()
{
    uart_init(115200);

    printf("hello, sw interrupt!!!\n");

    global_irq_enable(true);
    soft_irq_enable(true);

    while (1) {
        riscv_trigger_soft_irq();
        delay_ms(100);
    }
}

void __attribute__((interrupt)) isr_riscv_machine_soft_irq()
{
    printf("sw int\n");
}
