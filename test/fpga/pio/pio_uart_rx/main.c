#include <stdint.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "uart_rx.h"
#include "utils.h"

#define RX_PIN  30
#define BAUD    115200

PIO pio;
uint32_t sm;

int main()
{
    uart_init(115200);

    printf("hello pio uart rx!!!\n");

    int offset = pio_add_program(&pio, &sm, &uart_rx_program);
    if (offset < 0) {
        printf("Not enough space for sm!\n");
        return -1;
    }

    pio_sm_config config = {0};
    pio_sm_config_set_wrap(&config, offset + uart_rx_wrap_bottom, offset + uart_rx_wrap_top);
    pio_sm_config_set_instr_offset(&config, offset);
    pio_sm_config_set_clkdiv(&config, 12000000 / 4 / BAUD, 0);
    pio_sm_config_set_in_pins(&config, RX_PIN);
    pio_sm_config_set_in_shift(&config, true, true, 8);

    pio_sm_set_consecutive_pindirs(pio, sm, RX_PIN, 1, false);

    pio_sm_init(pio, sm, 0, &config);
    pio_sm_set_enabled(pio, sm, true);

    while (1) {
        uint8_t data = pio_sm_get_blocking(pio, sm) >> 24;
        printf("%c", data);
    }
}
