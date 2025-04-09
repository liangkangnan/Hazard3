#ifndef _HAZARD3_IRQ_H
#define _HAZARD3_IRQ_H

#include "hazard3_csr.h"
#include "riscv_encode.h"
#include "stdint.h"
#include "stdbool.h"

enum exception_number {
    // Assigned to non-IRQ xcause values
    MIN_EXCEPTION_NUM       = 0,
    INSTR_ALIGN_EXCEPTION   = 0,  ///< Instruction fetch misaligned (never fires if C/Zca is present)
    INSTR_FAULT_EXCEPTION   = 1,  ///< Instruction fetch bus fault
    INSTR_ILLEGAL_EXCEPTION = 2,  ///< Invalid or illegal instruction
    EBREAK_EXCEPTION        = 3,  ///< ebreak was not caught by an external debugger
    LOAD_ALIGN_EXCEPTION    = 4,  ///< Load address not naturally aligned
    LOAD_FAULT_EXCEPTION    = 5,  ///< Load bus fault
    STORE_ALIGN_EXCEPTION   = 6,  ///< Store or AMO address not naturally aligned
    STORE_FAULT_EXCEPTION   = 7,  ///< Store or AMO bus fault
    ECALL_UMODE_EXCEPTION   = 8,  ///< ecall was executed in U-mode
    ECALL_SMODE_EXCEPTION   = 9,  ///< ecall was executed in S-mode
    ECALL_MMODE_EXCEPTION   = 11, ///< ecall was executed in M-mode
    MAX_EXCEPTION_NUM       = 11
};

enum riscv_vector_num {
    RISCV_VEC_MACHINE_EXCEPTION = 0,
    RISCV_VEC_MACHINE_SOFTWARE_IRQ = 3,
    RISCV_VEC_MACHINE_TIMER_IRQ = 7,
    RISCV_VEC_MACHINE_EXTERNAL_IRQ = 11,
};

enum external_irq_num {
    UART_IRQ_NUM        = 0,
    PIO0_IRQ_NUM        = 1,
    PIO1_IRQ_NUM        = 2,
    TIMER0_IRQ_NUM      = 3,
    DMA0_IRQ_NUM        = 4,
    DMA1_IRQ_NUM        = 5
};

// Should match processor configuration in testbench:
#define NUM_IRQS 32
#define MAX_PRIORITY 15

#define h3irq_array_read(csr, index) (read_set_csr(csr, (index)) >> 16)

#define h3irq_array_write(csr, index, data) (write_csr(csr, (index) | ((uint32_t)(data) << 16)))
#define h3irq_array_set(csr, index, data) (set_csr(csr, (index) | ((uint32_t)(data) << 16)))
#define h3irq_array_clear(csr, index, data) (clear_csr(csr, (index) | ((uint32_t)(data) << 16)))

typedef void (*irq_handler_t)(void);

static inline void h3irq_enable(unsigned int irq, bool enable) {
	if (enable) {
		h3irq_array_set(hazard3_csr_meiea, irq >> 4, 1u << (irq & 0xfu));
	}
	else {
		h3irq_array_clear(hazard3_csr_meiea, irq >> 4, 1u << (irq & 0xfu));
	}
}

static inline bool h3irq_pending(unsigned int irq) {
	return h3irq_array_read(hazard3_csr_meipa, irq >> 4) & (1u << (irq & 0xfu));
}

static inline void h3irq_force_pending(unsigned int irq, bool force) {
	if (force) {
		h3irq_array_set(hazard3_csr_meifa, irq >> 4, 1u << (irq & 0xfu));
	}
	else {
		h3irq_array_clear(hazard3_csr_meifa, irq >> 4, 1u << (irq & 0xfu));
	}
}

static inline bool h3irq_is_forced(unsigned int irq) {
	return h3irq_array_read(hazard3_csr_meifa, irq >> 4) & (1u << (irq & 0xfu));
}

// -1 for no IRQ
static inline int h3irq_get_current_irq() {
	uint32_t meicontext = read_csr(hazard3_csr_meicontext);
	return meicontext & 0x8000u ? -1 : (meicontext >> 4) & 0x1ffu;
}

static inline void h3irq_set_priority(unsigned int irq, uint32_t priority) {
	// Don't want read-modify-write, but no instruction for atomically writing
	// a bitfield. So, first drop priority to minimum, then set to the target
	// value. It should be safe to drop an IRQ's priority below its current
	// even from within that IRQ (but it is never safe to boost an IRQ when
	// it may already be in an older stack frame)
	h3irq_array_clear(hazard3_csr_meipra, irq >> 2, 0xfu << (4 * (irq & 0x3)));
	h3irq_array_set(hazard3_csr_meipra, irq >> 2, (priority & 0xfu) << (4 * (irq & 0x3)));
}

static inline irq_handler_t *get_external_irq_table() {
	extern uintptr_t __external_irq_vector_table;
	return (irq_handler_t *) &__external_irq_vector_table;
}

static inline void h3irq_set_external_irq_handler(uint32_t num, irq_handler_t handler) {
	get_external_irq_table()[num] = handler;
}

static inline irq_handler_t *get_exception_table() {
	extern uintptr_t __exception_vector_table;
	return (irq_handler_t *) &__exception_vector_table;
}

static inline void h3irq_set_exception_handler(uint32_t num, irq_handler_t handler) {
	get_exception_table()[num] = handler;
}

static inline irq_handler_t *get_vtable() {
	return (irq_handler_t *)(read_csr(mtvec) & ~0x3u);
}

static uint32_t encode_j_instruction(uintptr_t from, uintptr_t to) {
	intptr_t delta = (intptr_t) (to - from);
	return 0x6fu | riscv_encode_imm_j((uint32_t)delta);
}

static void irq_set_riscv_vector_handler(enum riscv_vector_num index, irq_handler_t handler) {
	irq_handler_t *vtable = get_vtable();
	vtable[index] = (irq_handler_t)encode_j_instruction((uintptr_t)&vtable[index], (uintptr_t)handler);
}

static inline void global_irq_enable(bool en) {
	// mstatus.mie
	if (en) {
		set_csr(mstatus, 0x8);
	}
	else {
		clear_csr(mstatus, 0x8);
	}
}

static inline void external_irq_enable(bool en) {
	// mie.meie
	if (en) {
		set_csr(mie, 0x800);
	}
	else {
		clear_csr(mie, 0x800);
	}
}

static inline void timer_irq_enable(bool en) {
	// mie.mtie
	if (en) {
		set_csr(mie, 0x080);
	}
	else {
		clear_csr(mie, 0x080);
	}
}

static inline void soft_irq_enable(bool en) {
	// mie.msie
	if (en) {
		set_csr(mie, 0x08);
	}
	else {
		clear_csr(mie, 0x08);
	}
}

#endif
