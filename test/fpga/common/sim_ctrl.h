#ifndef _SIM_CTRL_H_
#define _SIM_CTRL_H_

#include <stdint.h>

#define SIM_CTRL_BASE 0x80000000

typedef struct {
	volatile uint32_t print_char;
	volatile uint32_t print_u32;
	volatile uint32_t exit;
	uint32_t _pad0;
	volatile uint32_t set_softirq;
	volatile uint32_t clr_softirq;
	volatile uint32_t dump_wave;
} sim_ctrl_t;

#define sim_ctrl_io ((sim_ctrl_t *const)SIM_CTRL_BASE)

static inline void tb_putc(char c)
{
	sim_ctrl_io->print_char = (uint32_t)c;
}

static inline void tb_puts(const char *s)
{
	while (*s)
		tb_putc(*s++);
}

static inline void tb_put_u32(uint32_t x)
{
	sim_ctrl_io->print_u32 = x;
}

static inline void tb_exit(uint32_t ret)
{
	sim_ctrl_io->exit = ret;
}

static inline void tb_dump_wave_enable(uint32_t en)
{
	sim_ctrl_io->dump_wave = en;
}

#endif
