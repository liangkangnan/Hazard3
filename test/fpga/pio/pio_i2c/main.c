#include <stdint.h>
#include <stdbool.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "i2c.h"
#include "delay.h"
#include "utils.h"

#define I2C_SCL_PIN  0
#define I2C_SDA_PIN  1

#define SLAVE_ADDR  0xA0

PIO pio = pio0;
uint32_t sm = 0;

static void _i2c_start()
{
    // set pins 1 side 1
    pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 1) | pio_encode_sideset(1, 1)); // SCL=1, SDA=1
    // set pindirs 1
    pio_sm_exec(pio, sm, pio_encode_set(pio_pindirs, 1));
    // set pins 0 side 1
    pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 0) | pio_encode_sideset(1, 1)); // SCL=1, SDA=0
    // set pins 0 side 0
    pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 0) | pio_encode_sideset(1, 0)); // SCL=0, SDA=0
}

static void _i2c_stop()
{
    // set pins 0 side 0
    pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 0) | pio_encode_sideset(1, 0)); // SCL=0, SDA=0
    // set pindirs 1
    pio_sm_exec(pio, sm, pio_encode_set(pio_pindirs, 1));
    // set pins 0 side 1
    pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 0) | pio_encode_sideset(1, 1)); // SCL=1, SDA=0
    // set pins 1 side 1
    pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 1) | pio_encode_sideset(1, 1)); // SCL=1, SDA=1
}

// return: 0: ACK, 1: NACK
static uint8_t _i2c_write(uint8_t val)
{
    uint32_t data;

    data = val << 23;

    pio_sm_put_blocking(pio, sm, data);

    return (!!pio_sm_get_blocking(pio, sm));
}

static uint8_t _i2c_read()
{
    uint32_t data;

    data = 1 << 31;

    pio_sm_put_blocking(pio, sm, data);

    return pio_sm_get_blocking(pio, sm);
}

uint32_t i2c_write(uint8_t slave_addr, uint32_t reg_addr, uint8_t *buf, uint32_t num)
{
    uint32_t i;

    _i2c_start();
    _i2c_write(slave_addr);
    _i2c_write(reg_addr);
    for (i = 0; i < num; i++)
        _i2c_write(buf[i]);
    _i2c_stop();

    return num;
}

uint32_t i2c_read(uint8_t slave_addr, uint32_t reg_addr, uint8_t *buf, uint32_t num)
{
    uint32_t i;

    _i2c_start();
    _i2c_write(slave_addr);
    _i2c_write(reg_addr);
    _i2c_stop();

    _i2c_start();
    _i2c_write(slave_addr | 0x1);
    for (i = 0; i < num; i++)
        buf[i] = _i2c_read();
    _i2c_stop();

    return num;
}

int main()
{
    uart_init(115200);

    printf("hello pio i2c!!!\n");

    pio_sm_set_enabled(pio, sm, false);

    pio_add_program_at_offset(pio, &i2c_program, 0);

    pio_sm_config config;
    pio_sm_config_set_wrap(&config, i2c_wrap_bottom, i2c_wrap_top);
    pio_sm_config_set_clkdiv(&config, 60, 0);
    pio_sm_config_set_in_pins(&config, I2C_SDA_PIN);
    pio_sm_config_set_in_shift(&config, false, false, 8);
    pio_sm_config_set_out_pins(&config, I2C_SDA_PIN, 1);
    pio_sm_config_set_out_shift(&config, false, false, 8);
    pio_sm_config_set_sideset(&config, I2C_SCL_PIN, 1, true, false);
    pio_sm_config_set_set_pins(&config, I2C_SDA_PIN, 1);
    pio_sm_init(pio, sm, 0, &config);

    pio_sm_set_consecutive_pindirs(pio, sm, I2C_SCL_PIN, 2, true);
    // set pins 1 side 1
    pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 1) | pio_encode_sideset(1, 1));

    printf("pio i2c started\n");

    pio_sm_set_enabled(pio, sm, true);

    uint8_t write_data[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
    i2c_write(SLAVE_ADDR, 0x00, write_data, 5);

    delay_ms(200);

    uint8_t read_data[5];
    i2c_read(SLAVE_ADDR, 0x00, read_data, 5);

    for (uint8_t i = 0; i < 5; i++)
        printf("read[%d]=0x%x\n", i, read_data[i]);

    while (1) {

    }
}
