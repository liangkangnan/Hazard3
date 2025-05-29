#include <stdint.h>
#include <stdbool.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "spi.h"
#include "utils.h"

#define SPI_CLK_PIN  22
#define SPI_MOSI_PIN 23
#define SPI_MISO_PIN 19
#define SPI_CS_PIN   18

PIO pio;
uint32_t sm;

void spi_cs(uint8_t level)
{
    if (level)
        pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 1));
    else
        pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 0));
}

uint8_t spi_write8_and_read8(uint8_t data)
{
    pio_sm_put_blocking(pio, sm, data << 24);
    data = pio_sm_get_blocking(pio, sm);

    return data;
}


int main()
{
    uart_init(115200);

    printf("hello pio spi!!!\n");

    int offset = pio_add_program(&pio, &sm, &spi_program);
    if (offset < 0) {
        printf("Not enough space for sm!\n");
        return -1;
    }

    pio_sm_config config = {0};
    pio_sm_config_set_wrap(&config, offset + spi_wrap_bottom, offset + spi_wrap_top);
    pio_sm_config_set_instr_offset(&config, offset);
    pio_sm_config_set_clkdiv(&config, 4, 0);
    pio_sm_config_set_in_pins(&config, SPI_MISO_PIN);
    pio_sm_config_set_in_shift(&config, false, false, 8);
    pio_sm_config_set_out_pins(&config, SPI_MOSI_PIN, 1);
    pio_sm_config_set_out_shift(&config, false, false, 8);
    pio_sm_config_set_sideset(&config, SPI_CLK_PIN, 1, true, false);
    pio_sm_config_set_set_pins(&config, SPI_CS_PIN, 1);
    pio_sm_init(pio, sm, 0, &config);

    pio_sm_set_consecutive_pindirs(pio, sm, SPI_CLK_PIN, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, SPI_CS_PIN, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, SPI_MOSI_PIN, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, SPI_MISO_PIN, 1, false);

    spi_cs(1);

    printf("pio spi started\n");

    pio_sm_set_enabled(pio, sm, true);

    spi_cs(0);
    spi_write8_and_read8(0x9F);
    uint8_t read = spi_write8_and_read8(0xFF);
    spi_cs(1);

    if (read == 0xef)
        printf("read = 0x%x\n", read);
    else
        printf("read failed\n");

    while (1) {

    }
}
