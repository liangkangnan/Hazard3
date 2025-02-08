#ifndef _PIO_I2C_H_
#define _PIO_I2C_H_

#define i2c_wrap_target 0
#define i2c_wrap        17

#define i2c_wrap_bottom i2c_wrap_target
#define i2c_wrap_top    i2c_wrap

static const uint16_t i2c_program_instructions[] = {
    0x80a0,
    0x6021,
    0xe047,
    0x002c,
    0xf080,
    0x5801,
    0x1085,
    0xf081,
    0xf000,
    0xb842,
    0x8020,
    0x0000,
    0xf081,
    0x7001,
    0x188d,
    0xf080,
    0x5801,
    0x8020,
};

static const struct pio_program i2c_program = {
    .instructions = i2c_program_instructions,
    .length = sizeof(i2c_program_instructions) / sizeof(i2c_program_instructions[0]),
};

#endif
