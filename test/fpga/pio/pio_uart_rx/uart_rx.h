#ifndef _PIO_UART_RX_H_
#define _PIO_UART_RX_H_

#define uart_rx_wrap_target 0
#define uart_rx_wrap        3

#define uart_rx_wrap_bottom uart_rx_wrap_target
#define uart_rx_wrap_top    uart_rx_wrap

static const uint16_t uart_rx_program_instructions[] = {
    0x2020,
    0xe327,
    0x4001,
    0x0242,
};

static const struct pio_program uart_rx_program = {
    .instructions = uart_rx_program_instructions,
    .length = sizeof(uart_rx_program_instructions) / sizeof(uart_rx_program_instructions[0]),
};

#endif
