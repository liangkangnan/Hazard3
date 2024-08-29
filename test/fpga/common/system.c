#include <stdint.h>
#include "uart_drv.h"
#include "xprintf.h"
#include "hazard3_csr.h"


extern void runtime_init_per_core_h3_irq_registers(void);

void runtime_init()
{
    runtime_init_per_core_h3_irq_registers();
}
