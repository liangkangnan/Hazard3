#include <stdint.h>

#include "hazard3_irq.h"
#include "uart_drv.h"
#include "mach_timer_drv.h"
#include "xprintf.h"

// 1s
#define TIMER_INTERVAL (1000 * 1000)

static volatile uint8_t irq_assert;

int main()
{
	uart_init(115200);
    xdev_out(uart_putc);

    xprintf("hello timer!\n");

    global_irq_enable(true);
    mach_timer_irq_enable(true);

    mach_timer_set_time(0);
    mach_timer_set_cmp_time(TIMER_INTERVAL);

    irq_assert = 0;

    while (1) {
        if (irq_assert) {
            irq_assert = 0;
            xprintf("mach timer alarm\n");
        }
    }

	return 0;
}

void __attribute__((interrupt)) isr_machine_timer()
{
    mach_timer_set_time(0);

    irq_assert = 1;
}
