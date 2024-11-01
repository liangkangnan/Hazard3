#include <stdint.h>

#include "hazard3_irq.h"
#include "uart_drv.h"

#define UART_IRQ_NUM         0
#define UART_IRQ_PRIORITY    3

void uart_irq_handler();

int main()
{
	uart_init(115200);

    uart_puts("hello uart\n");

    global_irq_enable(true);
    external_irq_enable(true);

    h3irq_enable(UART_IRQ_NUM, true);
    h3irq_set_priority(UART_IRQ_NUM, UART_IRQ_PRIORITY);
    h3irq_set_external_irq_handler(UART_IRQ_NUM, uart_irq_handler);

    uart_txfifo_not_full_irq_enable(true);
    uart_rxfifo_not_empty_irq_enable(true);

    while (1) {

    }

	return 0;
}

void uart_irq_handler()
{
    if (uart_txfifo_not_full_irq_is_enabled() && (!uart_txfifo_full())) {
        uart_puts("uart tx fifo not full irq\n");
        uart_txfifo_not_full_irq_enable(false);
    }

    while (uart_rxfifo_not_empty_irq_is_enabled() && (!uart_rxfifo_empty())) {
        uart_putc(uart_getc());
    }

    //uart_puts("uart isr\n");
}
