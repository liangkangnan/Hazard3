#ifndef _UART_DRV_H_
#define _UART_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#include "uart_regs.h"

#define UART_MODULE_CLK (12000000)

#define UART_BASE_ADDR  (0x40004000)

#ifndef PRINTF_BUF_SIZE
#define PRINTF_BUF_SIZE 256
#endif

typedef struct {
    volatile uint32_t CSR;
    volatile uint32_t DIV;
    volatile uint32_t FSTAT;
    volatile uint32_t TX;
    volatile uint32_t RX;
} uart_t;

#define UART ((uart_t *)UART_BASE_ADDR)

void uart_init(uint32_t baud);
void uart_putc(char c);
uint8_t uart_getc();
void uart_puts(const char *s);
void uart_txfifo_not_full_irq_enable(bool en);
bool uart_txfifo_not_full_irq_is_enabled();
void uart_rxfifo_not_empty_irq_enable(bool en);
bool uart_rxfifo_not_empty_irq_is_enabled();
bool uart_txfifo_full();
bool uart_rxfifo_empty();

#endif
