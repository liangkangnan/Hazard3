#ifndef _PIO_WS2812_H_
#define _PIO_WS2812_H_

#define ws2812_wrap_target 0
#define ws2812_wrap        0

#define ws2812_wrap_bottom ws2812_wrap_target
#define ws2812_wrap_top    ws2812_wrap

static const uint16_t ws2812_program_instructions[] = {
    0x6001,
};

static const struct pio_program ws2812_program = {
    .instructions = ws2812_program_instructions,
    .length = sizeof(ws2812_program_instructions) / sizeof(ws2812_program_instructions[0]),
};

#endif
