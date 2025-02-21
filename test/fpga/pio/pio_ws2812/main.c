#include <stdint.h>
#include <stdbool.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "ws2812.h"


#define RGB(R, G, B) (((B) << 16) | ((R) << 8) | (G))

#define COLOR_RED       RGB(255, 0  , 0)
#define COLOR_GREEN     RGB(0  , 255, 0)
#define COLOR_BLUE      RGB(0  , 0  , 255)

#define LED_NUM     3

PIO pio = pio0;
uint32_t sm = 0;


void ws2812_set_color(uint32_t led_color[])
{
    uint32_t data[LED_NUM * 3];
    uint32_t i, j, k;
    uint8_t byte, bits;
    uint32_t color;

    // LED_NUM个LED
    for (k = 0; k < LED_NUM; k++) {
        color = led_color[k];
        // 每种颜色需要3个字节
        for (i = 0; i < 3; i++) {
            byte = (color >> (i * 8)) & 0xff;
            data[i + k * 3] = 0;
            // 一个字节8个bit
            for (j = 0; j < 8; j++) {
                if (byte & 0x80)
                    bits = 0x7;
                else
                    bits = 0x1;
                data[i + k * 3] |= bits << (j * 4);
                byte = byte << 1;
            }
        }
    }

    for (i = 0; i < LED_NUM * 3; i++)
        pio_sm_put_blocking(pio, sm, data[i]);
}

int main()
{
    uart_init(115200);

    printf("hello pio ws2812!!!\n");

    pio_sm_config config = {0};
    pio_sm_config_set_wrap(&config, ws2812_wrap_bottom, ws2812_wrap_top);
    // set clk = 3MHz
    pio_sm_config_set_clkdiv(&config, 4, 0);
    pio_add_program_at_offset(pio, &ws2812_program, 0);

    pio_sm_config_set_out_pins(&config, 2, 1);
    pio_sm_config_set_out_shift(&config, true, true, 32);
    pio_sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_TX);
    pio_sm_init(pio, sm, 0, &config);

    pio_sm_set_consecutive_pindirs(pio, sm, 2, 1, true);

    printf("pio ws2812 started\n");

    pio_sm_set_enabled(pio, sm, true);

    uint32_t color[3] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE};
    ws2812_set_color(color);

    while (1) {

    }
}
