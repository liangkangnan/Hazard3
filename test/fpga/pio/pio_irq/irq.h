#ifndef _PIO_IRQ_H_
#define _PIO_IRQ_H_

#define irq_wrap_target 0
#define irq_wrap        4

#define irq_wrap_bottom irq_wrap_target
#define irq_wrap_top    irq_wrap

static const uint16_t irq_program_instructions[] = {
    0x80a0,
    0xe05f,
    0x6001,
    0x0082,
    0xc020,
};

static const struct pio_program irq_program = {
    .instructions = irq_program_instructions,
    .length = sizeof(irq_program_instructions) / sizeof(irq_program_instructions[0]),
};

#endif
