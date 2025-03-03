#ifndef _PIO_NEC_TRANSMIT_H_
#define _PIO_NEC_TRANSMIT_H_

#define nec_transmit_wrap_target 0
#define nec_transmit_wrap        0

#define nec_transmit_wrap_bottom nec_transmit_wrap_target
#define nec_transmit_wrap_top    nec_transmit_wrap

static const uint16_t nec_transmit_program_instructions[] = {
    0x6001,
};

static const struct pio_program nec_transmit_program = {
    .instructions = nec_transmit_program_instructions,
    .length = sizeof(nec_transmit_program_instructions) / sizeof(nec_transmit_program_instructions[0]),
};

#endif
