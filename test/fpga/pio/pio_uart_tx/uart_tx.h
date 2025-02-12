#ifndef _PIO_UART_TX_H_
#define _PIO_UART_TX_H_

#define uart_tx_wrap_target 0
#define uart_tx_wrap        3

#define uart_tx_wrap_bottom uart_tx_wrap_target
#define uart_tx_wrap_top    uart_tx_wrap

static const uint16_t uart_tx_program_instructions[] = {
    0x9ba0,
    0xf327,
    0x6001,
    0x0242,
};

static const struct pio_program uart_tx_program = {
    .instructions = uart_tx_program_instructions,
    .length = sizeof(uart_tx_program_instructions) / sizeof(uart_tx_program_instructions[0]),
};

#endif
