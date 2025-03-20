#ifndef _PIO_I2S_H_
#define _PIO_I2S_H_

#define i2s_wrap_target 0
#define i2s_wrap        7

#define i2s_wrap_bottom i2s_wrap_target
#define i2s_wrap_top    i2s_wrap

static const uint16_t i2s_program_instructions[] = {
    0x7001,
    0x1480,
    0x7801,
    0xbc41,
    0x7801,
    0x1c84,
    0x7001,
    0xb441,
};

static const struct pio_program i2s_program = {
    .instructions = i2s_program_instructions,
    .length = sizeof(i2s_program_instructions) / sizeof(i2s_program_instructions[0]),
};

#endif
