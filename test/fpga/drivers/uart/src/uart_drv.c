#include <stdint.h>
#include <stdbool.h>

#include "system.h"
#include "uart_drv.h"

void uart_init(uint32_t baud)
{
    UART->CSR |= 1;
    UART->DIV = (get_uart_clock_hz() / 8 / baud) << 4;
}

void uart_putc(char c)
{
    while (UART->FSTAT & (1 << UART_FSTAT_TXFULL_LSB));
	UART->TX = (uint8_t)c;
}

uint8_t uart_getc()
{
    return UART->RX;
}

void uart_puts(const char *s)
{
	while (*s)
		uart_putc(*s++);
}

void uart_txfifo_not_full_irq_enable(bool en)
{
    if (en)
        UART->CSR |= 1 << UART_CSR_TXIE_LSB;
    else
        UART->CSR &= ~(1 << UART_CSR_TXIE_LSB);
}

bool uart_txfifo_not_full_irq_is_enabled()
{
    if (UART->CSR & (1 << UART_CSR_TXIE_LSB))
        return true;
    else
        return false;
}

void uart_rxfifo_not_empty_irq_enable(bool en)
{
    if (en)
        UART->CSR |= 1 << UART_CSR_RXIE_LSB;
    else
        UART->CSR &= ~(1 << UART_CSR_RXIE_LSB);
}

bool uart_rxfifo_not_empty_irq_is_enabled()
{
    if (UART->CSR & (1 << UART_CSR_RXIE_LSB))
        return true;
    else
        return false;
}

bool uart_txfifo_full()
{
    if (UART->FSTAT & (1 << UART_FSTAT_TXFULL_LSB))
        return true;
    else
        return false;
}

bool uart_rxfifo_empty()
{
    if (UART->FSTAT & (1 << UART_FSTAT_RXEMPTY_LSB))
        return true;
    else
        return false;
}

// for printf
void _putchar(char character)
{
    uart_putc(character);
}

int __io_putchar(int ch)
{
    uart_putc(ch);
}

int __io_getchar()
{
    return 0;
}
