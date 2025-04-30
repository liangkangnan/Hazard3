#ifndef _PIO_PWM2_H_
#define _PIO_PWM2_H_

#define pwm2_wrap_target 0
#define pwm2_wrap        5

#define pwm2_wrap_bottom pwm2_wrap_target
#define pwm2_wrap_top    pwm2_wrap

static const uint16_t pwm2_program_instructions[] = {
    0x98a0,
    0xa027,
    0x0042,
    0x90a0,
    0xa027,
    0x0045,
};

static const struct pio_program pwm2_program = {
    .instructions = pwm2_program_instructions,
    .length = sizeof(pwm2_program_instructions) / sizeof(pwm2_program_instructions[0]),
};

#endif
