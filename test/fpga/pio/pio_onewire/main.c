#include <stdint.h>
#include <stdbool.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "onewire.h"
#include "delay.h"
#include "utils.h"


#define ONEWIRE_PIN  3

PIO pio;
uint32_t sm;


static uint8_t ds18b20_crc8(uint8_t *data, uint8_t len)
{
    uint8_t crc = 0, i, j;

    for (i = 0; i < len; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x01)
                crc = (crc >> 1) ^ 0x8C;
            else
                crc >>= 1;
        }
    }

    return crc;
}

static uint8_t onewire_init()
{
    uint8_t ret = 0;

    // nop side 1
    pio_sm_exec(pio, sm, pio_encode_nop() | pio_encode_sideset(1, 1));

    delay_us(500);

    // nop side 0
    pio_sm_exec(pio, sm, pio_encode_nop() | pio_encode_sideset(1, 0));

    delay_us(30);

    uint64_t start = riscv_get_cycle_time();
    while (riscv_get_cycle_time() < ((uint64_t)(start + 200 * 12))) {
        if (!(pio_gpio_data_get(pio) & (1 << ONEWIRE_PIN))) {
            ret = 1;
            break;
        }
    }

    while (!(pio_gpio_data_get(pio) & (1 << ONEWIRE_PIN)));

    return ret;
}

static void onewire_write(uint8_t val)
{
    pio_sm_put_blocking(pio, sm, val);
    pio_sm_get_blocking(pio, sm);
}

static uint8_t onewire_read()
{
    pio_sm_put_blocking(pio, sm, 0xff);
    return (pio_sm_get_blocking(pio, sm) >> 24);
}

int main()
{
    uart_init(115200);

    printf("hello pio onewire!!!\n");

    int offset = pio_add_program(&pio, &sm, &onewire_program);
    if (offset < 0) {
        printf("Not enough space for sm!\n");
        return -1;
    }

    pio_sm_config config = {0};
    pio_sm_config_set_wrap(&config, offset + onewire_wrap_bottom, offset + onewire_wrap_top);
    pio_sm_config_set_instr_offset(&config, offset);
    pio_sm_config_set_clkdiv(&config, 12, 0);
    pio_sm_config_set_in_pins(&config, ONEWIRE_PIN);
    pio_sm_config_set_in_shift(&config, true, true, 8);
    pio_sm_config_set_out_shift(&config, true, false, 8);
    pio_sm_config_set_sideset(&config, ONEWIRE_PIN, 1, true, true);
    pio_sm_init(pio, sm, 0, &config);

    pio_sm_set_consecutive_pindirs(pio, sm, ONEWIRE_PIN, 1, false);
    pio_gpio_data_bits_clr(pio, ONEWIRE_PIN);

    pio_sm_set_enabled(pio, sm, true);

    printf("pio onewire started\n");

    uint8_t data[9];

    while (1) {
        if (onewire_init()) {
            onewire_write(0xcc);
            onewire_write(0x44);
            delay_ms(800);
            if (onewire_init()) {
                onewire_write(0xcc);
                onewire_write(0xBE);
                for (uint32_t i = 0; i < 9; i++)
                    data[i] = onewire_read();
                if (!ds18b20_crc8(data, 9))
                    printf("temp = %d\n", ((int16_t)((data[1] << 8) | data[0])) / 16);
                else
                    printf("read error!!!\n");
            } else {
                printf("init fail 2\n");
            }
        } else {
            printf("init fail 1\n");
            delay_ms(1000);
        }
    }
}
