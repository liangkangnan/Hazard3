#ifndef _PIO_NEC_RECEIVE_H_
#define _PIO_NEC_RECEIVE_H_

#define nec_receive_wrap_target 0
#define nec_receive_wrap        15

#define nec_receive_wrap_bottom nec_receive_wrap_target
#define nec_receive_wrap_top    nec_receive_wrap

static const uint16_t nec_receive_program_instructions[] = {
    0xe03e,
    0x2020,
    0x00ce,
    0x0042,
    0xa0c3,
    0x20a0,
    0xae42,
    0xae42,
    0xac42,
    0x00c0,
    0xa0c2,
    0x8020,
    0x20a0,
    0x0000,
    0xae42,
    0x4001,
};

static const struct pio_program nec_receive_program = {
    .instructions = nec_receive_program_instructions,
    .length = sizeof(nec_receive_program_instructions) / sizeof(nec_receive_program_instructions[0]),
};

#endif
