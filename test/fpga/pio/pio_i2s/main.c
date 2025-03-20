#include <stdint.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "system.h"
#include "pio_i2s.h"

#define BCLK_PIN  0
#define LRCLK_PIN (BCLK_PIN + 1)
#define DOUT_PIN  2

#define SAMPLE_RATE  48000
#define CHANNELS     2
#define BITS         32

PIO pio;
uint32_t sm;

int main()
{
    uart_init(115200);

    printf("hello pio i2s!!!\n");

    int offset = pio_add_program(&pio, &sm, &i2s_program);
    if (offset < 0) {
        printf("Not enough space for sm!\n");
        return -1;
    }

    pio_sm_config config = {0};
    pio_sm_config_set_wrap(&config, offset, offset + i2s_program.length - 1);
    //pio_sm_config_set_clkdiv(&config, get_core_clock_hz() / 2 / (SAMPLE_RATE * CHANNELS * BITS) + 0.5f, 0);
    pio_sm_config_set_clkdiv(&config, 2, 0);
    pio_sm_config_set_out_pins(&config, DOUT_PIN, 1);
    pio_sm_config_set_out_shift(&config, false, true, BITS);
    pio_sm_config_set_sideset(&config, BCLK_PIN, 2, true, false);

    pio_sm_set_consecutive_pindirs(pio, sm, BCLK_PIN, 3, true);

    // set x, 30 side 0
    pio_sm_exec(pio, sm, pio_encode_set(pio_x, BITS - 2) | pio_encode_sideset(2, 0));
    // set y, 30 side 0
    pio_sm_exec(pio, sm, pio_encode_set(pio_y, BITS - 2) | pio_encode_sideset(2, 0));

    pio_sm_init(pio, sm, offset, &config);
    pio_sm_set_enabled(pio, sm, true);

    printf("pio i2s started\n");

    while (1) {
        pio_sm_put_blocking(pio, sm, 0x12345678);
        pio_sm_put_blocking(pio, sm, 0x87654321);
    }
}
