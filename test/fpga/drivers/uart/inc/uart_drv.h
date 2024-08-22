#ifndef _UART_DRV_H_
#define _UART_DRV_H_

#include "uart_regs.h"

#define UART_MODULE_CLK (24000000)

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
void uart_puts(const char *s);

#endif
