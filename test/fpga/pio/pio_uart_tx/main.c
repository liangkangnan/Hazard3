#include <stdint.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "uart_tx.h"
#include "delay.h"


#define TX_PIN  30
#define BAUD    115200

PIO pio;
uint32_t sm;

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

    int offset = pio_add_program(&pio, &sm, &uart_tx_program);
    if (offset < 0) {
        printf("Not enough space for sm!\n");
        return -1;
    }

    pio_sm_config config = {0};
    pio_sm_config_set_wrap(&config, offset + uart_tx_wrap_bottom, offset + uart_tx_wrap_top);
    pio_sm_config_set_instr_offset(&config, offset);
    pio_sm_config_set_clkdiv(&config, 12000000 / 4 / BAUD, 0);
    pio_sm_config_set_sideset(&config, TX_PIN, 1, true, false);
    pio_sm_config_set_out_pins(&config, TX_PIN, 1);
    pio_sm_config_set_out_shift(&config, true, false, 8);

    pio_sm_init(pio, sm, 0, &config);

    // nop side 1
    pio_sm_exec(pio, sm, pio_encode_nop() | pio_encode_sideset(1, 1));
    pio_sm_set_consecutive_pindirs(pio, sm, TX_PIN, 1, true);

    pio_sm_set_enabled(pio, sm, true);

    while (1) {
        pio_uart_send("hello pio\n", 10);
        delay_ms(1000);
    }
}
