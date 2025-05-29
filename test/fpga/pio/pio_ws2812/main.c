#include <stdint.h>
#include <stdbool.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "ws2812.h"
#include "delay.h"


#define RGB(R, G, B) (((B) << 16) | ((R) << 8) | (G))

#define COLOR_RED       RGB(255, 0  , 0)
#define COLOR_GREEN     RGB(0  , 255, 0)
#define COLOR_BLUE      RGB(0  , 0  , 255)

#define COLOR_RED_LESS       RGB(63, 0 , 0)
#define COLOR_GREEN_LESS     RGB(0 , 63, 0)
#define COLOR_BLUE_LESS      RGB(0 , 0 , 63)

#define COLOR_OFF            RGB(0, 0, 0)

#define LED_NUM     3

#define WS2812_IO   2

PIO pio;
uint32_t sm;


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

    int offset = pio_add_program(&pio, &sm, &ws2812_program);
    if (offset < 0) {
        printf("Not enough space for sm!\n");
        return -1;
    }

    pio_sm_config config = {0};
    pio_sm_config_set_wrap(&config, offset + ws2812_wrap_bottom, offset + ws2812_wrap_top);
    pio_sm_config_set_instr_offset(&config, offset);
    // set clk = 3MHz
    pio_sm_config_set_clkdiv(&config, 4, 0);

    pio_sm_config_set_out_pins(&config, WS2812_IO, 1);
    pio_sm_config_set_out_shift(&config, true, true, 32);
    pio_sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_TX);
    pio_sm_init(pio, sm, 0, &config);

    pio_sm_set_consecutive_pindirs(pio, sm, WS2812_IO, 1, true);

    printf("pio ws2812 started\n");

    pio_sm_set_enabled(pio, sm, true);

    uint32_t color[3] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE};
    uint32_t color_less[3] = {COLOR_RED_LESS, COLOR_GREEN_LESS, COLOR_BLUE_LESS};
    uint32_t color_off[3] = {COLOR_OFF, COLOR_OFF, COLOR_OFF};
    int32_t i = 20;

    while (i--) {
        ws2812_set_color(color);
        delay_ms(3000);
        ws2812_set_color(color_less);
        delay_ms(3000);
    }

    ws2812_set_color(color_off);

    while (1);
}
