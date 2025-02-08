#include <stdint.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "printf.h"
#include "multi_sm.h"


PIO pio = pio0;
uint32_t sm0 = 0;
uint32_t sm1 = 1;

int main()
{
    uart_init(115200);

    printf("hello pio multi sm!!!\n");

    pio_sm_config config0;
    pio_sm_config_set_set_pins(&config0, 8, 1);
    pio_sm_config_set_wrap(&config0, multi_sm_wrap_bottom, multi_sm_wrap_top);
    pio_sm_config_set_clkdiv(&config0, 12000000, 0);
    pio_add_program_at_offset(pio, &multi_sm_program, 0);

    pio_sm_set_consecutive_pindirs(pio, sm0, 8, 1, true);

    pio_sm_init(pio, sm0, 0, &config0);
    pio_sm_set_enabled(pio, sm0, true);

    pio_sm_config config1;
    pio_sm_config_set_set_pins(&config1, 9, 1);
    pio_sm_config_set_wrap(&config1, multi_sm_wrap_bottom + 2, multi_sm_wrap_top + 2);
    pio_sm_config_set_clkdiv(&config1, 6000000, 0);
    pio_add_program_at_offset(pio, &multi_sm_program, 2);

    pio_sm_set_consecutive_pindirs(pio, sm1, 9, 1, true);

    pio_sm_init(pio, sm1, 2, &config1);
    pio_sm_set_enabled(pio, sm1, true);

    printf("pio multi sm started\n");

    while (1);
}
