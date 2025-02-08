#ifndef _PIO_MULTI_SM_H_
#define _PIO_MULTI_SM_H_

#define multi_sm_wrap_target 0
#define multi_sm_wrap        1

#define multi_sm_wrap_bottom multi_sm_wrap_target
#define multi_sm_wrap_top    multi_sm_wrap

static const uint16_t multi_sm_program_instructions[] = {
    0xe001,
    0xe000,
};

static const struct pio_program multi_sm_program = {
    .instructions = multi_sm_program_instructions,
    .length = sizeof(multi_sm_program_instructions) / sizeof(multi_sm_program_instructions[0]),
};

#endif
