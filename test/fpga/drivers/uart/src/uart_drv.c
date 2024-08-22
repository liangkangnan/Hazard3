#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "uart_drv.h"

void uart_init(uint32_t baud)
{
    UART->CSR |= 1;
    UART->DIV = (UART_MODULE_CLK / 8 / baud) << 4;
}

void uart_putc(char c)
{
    while (UART->FSTAT & (1 << UART_FSTAT_TXFULL_LSB));
	UART->TX = (uint8_t)c;
}

void uart_puts(const char *s)
{
	while (*s)
		uart_putc(*s++);
}
