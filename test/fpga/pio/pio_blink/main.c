#include <stdint.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "printf.h"
#include "blink.h"

#define GPIO_BASE  8
#define GPIO_COUNT 4

PIO pio;
uint32_t sm;

int main()
{
    uart_init(115200);

    printf("hello pio blink!!!\n");

    int offset = pio_add_program(&pio, &sm, &blink_program);
    if (offset < 0) {
        printf("Not enough space for sm!\n");
        return -1;
    }

    pio_sm_config config = {0};
    pio_sm_config_set_set_pins(&config, GPIO_BASE, GPIO_COUNT);
    pio_sm_config_set_wrap(&config, offset + blink_wrap_bottom, offset + blink_wrap_top);
    pio_sm_config_set_instr_offset(&config, offset);
    pio_sm_config_set_clkdiv(&config, 12000000, 0);

    pio_sm_set_consecutive_pindirs(pio, sm, GPIO_BASE, GPIO_COUNT, true);

    pio_sm_init(pio, sm, 0, &config);
    pio_sm_set_enabled(pio, sm, true);

    printf("pio blink started\n");

    while (1);
}
