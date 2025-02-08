#include <stdint.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "printf.h"
#include "blink.h"

PIO pio = pio0;
uint32_t sm = 0;

int main()
{
    uart_init(115200);

    printf("hello pio blink!!!\n");

    pio_sm_config config;
    pio_sm_config_set_set_pins(&config, 8, 4);
    pio_sm_config_set_wrap(&config, 0, blink_program.length - 1);
    pio_sm_config_set_clkdiv(&config, 12000000, 0);
    pio_add_program_at_offset(pio, &blink_program, 0);

    pio_sm_set_consecutive_pindirs(pio, sm, 8, 4, true);

    pio_sm_init(pio, sm, 0, &config);
    pio_sm_set_enabled(pio, sm, true);

    printf("pio blink started\n");

    while (1);
}
