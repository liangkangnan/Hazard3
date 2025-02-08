#ifndef _PIO_PWM_H_
#define _PIO_PWM_H_

#define pwm_wrap_target 0
#define pwm_wrap        5

#define pwm_wrap_bottom pwm_wrap_target
#define pwm_wrap_top    pwm_wrap

static const uint16_t pwm_program_instructions[] = {
    0x9080,
    0xa047,
    0x0082,
    0x9880,
    0xa047,
    0x0085,
};

static const struct pio_program pwm_program = {
    .instructions = pwm_program_instructions,
    .length = sizeof(pwm_program_instructions) / sizeof(pwm_program_instructions[0]),
};

#endif
