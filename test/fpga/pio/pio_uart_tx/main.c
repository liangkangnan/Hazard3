#include <stdint.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "printf.h"
#include "uart_tx.h"


#define BAUD    115200

PIO pio = pio0;
uint32_t sm = 0;

void pio_uart_send(uint8_t *buf, uint32_t count)
{
    uint32_t i;

    for (i = 0; i < count; i++)
        pio_sm_put_blocking(pio, sm, buf[i]);
}

int main()
{
    uart_init(115200);

    printf("hello pio uart tx!!!\n");

    pio_sm_set_enabled(pio, sm, false);

    pio_sm_config config;
    pio_sm_config_set_wrap(&config, 0, uart_tx_program.length - 1);
    pio_sm_config_set_clkdiv(&config, 12000000 / 4 / BAUD, 0);
    pio_add_program_at_offset(pio, &uart_tx_program, 0);

    pio_sm_config_set_sideset(&config, 30, 1, true, false);
    pio_sm_config_set_out_pins(&config, 30, 1);
    pio_sm_config_set_out_shift(&config, true, false, 8);
    pio_sm_set_consecutive_pindirs(pio, sm, 30, 1, true);

    pio_sm_init(pio, sm, 0, &config);
    pio_sm_set_enabled(pio, sm, true);

    pio_uart_send("hello pio\n", 10);

    while (1);
}
