.globl _start;

_start:
.option push
.option norelax

.section .vectors

.macro VEC name:req
.p2align 2
j \name
.p2align 2
.endm

// ----------------------------------------------------------------------------
// Vector table (must be at least aligned to its size rounded up to power of 2)

.p2align 12
.vector_table:

// Single exception vector, also takes IRQs if vectoring is disabled

	VEC handle_exception

// Standard interrupts, if vectoring is enabled
// Note: global EIRQ does not fire. Instead we have 16 separate vectors

	// handle_exception ^^^ takes the slot where U-mode softirq would be
	VEC .halt
	VEC .halt
	VEC isr_machine_softirq
	VEC .halt
	VEC .halt
	VEC .halt
	VEC isr_machine_timer
	VEC .halt
	VEC .halt
	VEC .halt
	VEC isr_external_irq
	VEC .halt
	VEC .halt
	VEC .halt
	VEC .halt

// ----------------------------------------------------------------------------
// Reset handler

.reset_handler:
	// Set counters running, as they are off by default. This may trap if counters
	// are unimplemented, so catch the trap and continue.
	la a0, 1f
	csrw mtvec, a0
	csrci mcountinhibit, 0x5
.p2align 2
1:
	// Set up trap vector table. mtvec LSB enables vectoring
	la a0, .vector_table + 1
	csrw mtvec, a0

	// Put spare cores to sleep before setting up core 0 stack
	csrr a0, mhartid
	bnez a0, .core1_wait

	// Set up stack pointer before doing anything else
	la sp, _sp
    la gp, __global_pointer$

	/* Load data section */
	la a0, _data_lma
	la a1, _data
	la a2, _edata
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:

	/* Clear bss section */
	la a0, __bss_start
	la a1, _end
	bgeu a0, a1, 2f
1:
	sw zero, (a0)
	addi a0, a0, 4
	bltu a0, a1, 1b
2:

	li a0, 0
    li a1, 0
    call main

	j .halt

.core1_wait:
	// IRQs disabled, but soft IRQ unmasked -> soft IRQ will exit WFI.
	csrci mstatus, 0x8
	csrw mie, 0x8
.core1_wait_loop:
	wfi
	la a0, core1_entry_vector
	lw a0, (a0)
	beqz a0, .core1_wait_loop
	la sp, __stack_top - 0x10000
	jalr a0
.core1_finish:
	wfi
	j .core1_finish

.p2align 2
.global core1_entry_vector
core1_entry_vector:
	.word 0

.global _exit
_exit:
	j .halt

.global _sbrk
_sbrk:
	la a1, heap_ptr
	lw a2, (a1)
	add a0, a0, a2
	sw a0, (a1)
	mv a0, a2
	ret

.p2align 2
heap_ptr:
	.word _end

.global .halt
.halt:
	j .halt


.macro weak_handler name:req
.p2align 2
.global \name
.weak \name
\name:
	j .halt
_str_\name:
	.asciz "\name"
.endm

weak_handler handle_exception
weak_handler isr_machine_softirq
weak_handler isr_machine_timer
weak_handler isr_external_irq

