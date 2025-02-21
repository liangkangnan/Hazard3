#ifndef _PIO_ONEWIRE_H_
#define _PIO_ONEWIRE_H_

#define onewire_wrap_target 0
#define onewire_wrap        13

#define onewire_wrap_bottom onewire_wrap_target
#define onewire_wrap_top    onewire_wrap

static const uint16_t onewire_program_instructions[] = {
    0x80a0,
    0xf047,
    0x7021,
    0x1e29,
    0xf727,
    0x5401,
    0x1546,
    0x1082,
    0x1000,
    0xfd27,
    0x1d4a,
    0x5701,
    0x1182,
    0x1000,
};

static const struct pio_program onewire_program = {
    .instructions = onewire_program_instructions,
    .length = sizeof(onewire_program_instructions) / sizeof(onewire_program_instructions[0]),
};

#endif
