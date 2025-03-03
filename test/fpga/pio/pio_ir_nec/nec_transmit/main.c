#include <stdint.h>
#include <stdbool.h>

#include "system.h"
#include "delay.h"
#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "nec_transmit.h"
#include "binarybuffer.h"


#define OUT_PIN  31

PIO pio = pio0;
uint32_t sm = 0;


uint32_t nec_encode_frame(uint8_t address, uint8_t data)
{
    uint32_t frame;

    frame = address | (((~address) & 0xff) << 8) | (data << 16) | (((~data) & 0xff) << 24);

    printf("frame: 0x%08x\n", frame);

    return frame;
}

void nec_transmit(uint32_t frame)
{
    uint32_t data[8];
    uint32_t i;
    uint8_t *p;
    uint32_t bits;

    for (i = 0; i < 8; i++)
        data[i] = 0xffffffff;

    p = (uint8_t *)data;

    // prefix
    buf_set_u32(p, 0, 24, 0xff0000);
    bits = 24;

    for (i = 0; i < 32; i++) {
        if (frame & 0x1) {
            buf_set_u32(p, bits, 4, 0xE);
            bits += 4;
        } else {
            buf_set_u32(p, bits, 2, 0x2);
            bits += 2;
        }

        frame = frame >> 1;
    }
    // the last bit must pull down
    buf_set_u32(p, bits, 1, 0x0);

    printf("bits = %d\n", bits);
    for (i = 0; i < 8; i++)
        printf("put data[%d] = 0x%08x\n", i, data[i]);

    for (i = 0; i < 8; i++)
        pio_sm_put_blocking(pio, sm, data[i]);
}

void nec_transmit_repeat()
{
    pio_sm_put_blocking(pio, sm, 0xffef0000);
}

int main()
{
    uart_init(115200);

    printf("hello pio nec_transmit!!!\n");

    pio_sm_config config = {0};
    pio_sm_config_set_wrap(&config, 0, nec_transmit_program.length - 1);
    pio_sm_config_set_clkdiv(&config, get_core_clock_hz() / (1.0 / 562.5e-6), 0);
    pio_add_program_at_offset(pio, &nec_transmit_program, 0);

    pio_sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_TX);
    pio_sm_config_set_out_pins(&config, OUT_PIN, 1);
    pio_sm_config_set_out_shift(&config, true, true, 32);
    pio_sm_config_set_set_pins(&config, OUT_PIN, 1);

    pio_sm_init(pio, sm, 0, &config);

    // set pins, 1
    pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 1));
    pio_sm_set_consecutive_pindirs(pio, sm, OUT_PIN, 1, true);

    pio_sm_set_enabled(pio, sm, true);

    printf("pio nec_transmit started\n");

    uint8_t data = 0;

    while (1) {
        nec_transmit(nec_encode_frame(0x00, data++));
        delay_ms(300);
        nec_transmit_repeat();
        delay_ms(300);
    }
}
