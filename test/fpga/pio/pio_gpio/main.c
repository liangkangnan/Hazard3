#include <stdint.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "printf.h"
#include "delay.h"

PIO pio = pio0;

int main()
{
    uart_init(115200);

    printf("hello pio gpio!!!\n");

    pio_sm_set_enabled(pio0, 0, false);

    pio_gpio_dir_put(pio, 1 << 8);

    while (1) {
        pio_gpio_data_bits_set(pio, 1 << 8);
        delay_ms(1000);
        pio_gpio_data_bits_clr(pio, 1 << 8);
        delay_ms(1000);
    }
}
