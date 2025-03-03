#include <stdint.h>

#include "hazard3_irq.h"
#include "uart_drv.h"
#include "timer_drv.h"
#include "printf.h"

#define TIMER0_IRQ_PRIORITY    3

void timer0_irq_handler();

int main()
{
	uart_init(115200);

    printf("hello timer\n");

    global_irq_enable(true);
    external_irq_enable(true);

    h3irq_enable(TIMER0_IRQ_NUM, true);
    h3irq_set_priority(TIMER0_IRQ_NUM, TIMER0_IRQ_PRIORITY);
    h3irq_set_external_irq_handler(TIMER0_IRQ_NUM, timer0_irq_handler);

    timer_int_enable(TIMER0, true);
    timer_set_mode(TIMER0, TIMER_MODE_RELOAD);
    timer_set_div(TIMER0, 12);
    timer_set_expire_count(TIMER0, 1000000);
    timer_enable(TIMER0, true);

    while (1) {

    }
}

void timer0_irq_handler()
{
    timer_clear_int_pending(TIMER0);
    printf("timer irq.\n");
}
