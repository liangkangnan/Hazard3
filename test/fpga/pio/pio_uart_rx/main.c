#include <stdint.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "uart_rx.h"
#include "utils.h"

#define BAUD    115200

PIO pio = pio0;
uint32_t sm = 0;

int main()
{
    uart_init(115200);

    printf("hello pio uart rx!!!\n");

    pio_sm_set_enabled(pio, sm, false);

    pio_sm_config config;
    pio_sm_config_set_wrap(&config, 0, uart_rx_program.length - 1);
    pio_sm_config_set_clkdiv(&config, 12000000 / 4 / BAUD, 0);
    pio_add_program_at_offset(pio, &uart_rx_program, 0);

    pio_sm_config_set_in_pins(&config, 30);
    pio_sm_config_set_in_shift(&config, true, false, 8);

    pio_sm_set_consecutive_pindirs(pio, sm, 30, 1, false);

    pio_sm_init(pio, sm, 0, &config);
    pio_sm_set_enabled(pio, sm, true);

    while (1) {
        uint8_t data = pio_sm_get_blocking(pio, sm) >> 24;
        printf("%c", data);
    }
}
