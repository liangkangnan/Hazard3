#ifndef _PIO_BLINK_H_
#define _PIO_BLINK_H_

#define blink_wrap_target 0
#define blink_wrap        1

#define blink_wrap_bottom blink_wrap_target
#define blink_wrap_top    blink_wrap

static const uint16_t blink_program_instructions[] = {
    0xe00f,
    0xe000,
};

static const struct pio_program blink_program = {
    .instructions = blink_program_instructions,
    .length = sizeof(blink_program_instructions) / sizeof(blink_program_instructions[0]),
};

#endif
