#define RVCSR_MEICONTEXT_OFFSET         (0x00000be5)
#define RVCSR_MEICONTEXT_CLEARTS_BITS   (0x00000002)
#define RVCSR_MEINEXT_OFFSET            (0x00000be4)
#define RVCSR_MEINEXT_UPDATE_BITS       (0x00000001)
#define RVCSR_MEIFA_OFFSET              (0x00000be2)
#define RVCSR_MIE_MEIE_BITS             (0x00000800)
#define RVCSR_MSTATUS_MIE_BITS          (0x00000008)

// must be in RAM due to branches from vector table
.section .time_critical.external_irq

.p2align 2
// External IRQ dispatch, inlined into the last vector table slot.
.global isr_riscv_machine_external_irq
.weak isr_riscv_machine_external_irq
isr_riscv_machine_external_irq:
    // Save caller saves and exception return state whilst IRQs are disabled.
    // We can't be preempted during this time, but if a higher-priority IRQ
    // arrives before we read meinext, that will be the one we enter.
    addi sp, sp, -80
    sw ra,  0(sp)
    sw t0,  4(sp)
    sw t1,  8(sp)
    sw t2, 12(sp)
    sw a0, 16(sp)
    sw a1, 20(sp)
    sw a2, 24(sp)
    sw a3, 28(sp)
    sw a4, 32(sp)
    sw a5, 36(sp)
    sw a6, 40(sp)
    sw a7, 44(sp)
    sw t3, 48(sp)
    sw t4, 52(sp)
    sw t5, 56(sp)
    sw t6, 60(sp)
    csrr a0, mepc
    csrr a1, mstatus
    sw a0, 64(sp)
    sw a1, 68(sp)
save_meicontext:
    // Make sure to set meicontext.clearts to clear+save mie.msie/mtie along
    // with ext IRQ context. We don't let these preempt ext IRQs because they
    // clear meicontext.mreteirq, which breaks __get_current_exception().
    csrrsi a2, RVCSR_MEICONTEXT_OFFSET, RVCSR_MEICONTEXT_CLEARTS_BITS
    sw a2, 72(sp)

get_first_irq:
    // Sample the current highest-priority active IRQ (left-shifted by 2) from
    // meinext, and write 1 to meinext.update to update meicontext with the
    // preemption priority and IRQ number of this IRQ
    csrrsi a0, RVCSR_MEINEXT_OFFSET, RVCSR_MEINEXT_UPDATE_BITS
    // MSB will be set if there is no active IRQ at the current priority level
    bltz a0, no_more_irqs
dispatch_irq:
    // Preemption priority was configured by meinext update, so enable preemption:
    csrsi mstatus, 0x8
    // <- from this point we can be preempted by a higher-priority interrupt.
    // meinext is pre-shifted by 2, so only an add is required to index table
    lui a1, %hi(__soft_vector_table)
    add a1, a1, a0
    lw a1, %lo(__soft_vector_table)(a1)
    jalr ra, a1
    // Disable IRQs on returning so we can sample the next IRQ
    csrci mstatus, 0x8
get_next_irq:
    // Get the next-highest-priority IRQ that is active at this level. If
    // there is such an IRQ, update meicontext with new preemption priority.
    csrrsi a0, RVCSR_MEINEXT_OFFSET, RVCSR_MEINEXT_UPDATE_BITS
    // MSB will be set if there is no active IRQ at the current priority level
    bgez a0, dispatch_irq

no_more_irqs:
    // Restore saved context and return from IRQ
    lw a0, 64(sp)
    lw a1, 68(sp)
    lw a2, 72(sp)
    csrw mepc, a0
    csrw mstatus, a1
    csrw RVCSR_MEICONTEXT_OFFSET, a2
    lw ra,  0(sp)
    lw t0,  4(sp)
    lw t1,  8(sp)
    lw t2, 12(sp)
    // skip a0 for now
    lw a1, 20(sp)
    lw a2, 24(sp)
    lw a3, 28(sp)
    lw a4, 32(sp)
    lw a5, 36(sp)
    lw a6, 40(sp)
    lw a7, 44(sp)
    lw t3, 48(sp)
    lw t4, 52(sp)
    lw t5, 56(sp)
    lw t6, 60(sp)
    // Before popping the stack frame, check if there is a new IRQ, and if so,
    // abandon the mret and take the IRQ. This avoids a worst-case (restore ->
    // mret -> enter -> save) latency. Note since we have already restored
    // meicontext, we will have to re-save it, to re-clear mtie/msie.
check_irq_before_exit:
    csrr a0, RVCSR_MEINEXT_OFFSET
    bgez a0, save_meicontext
    lw a0, 16(sp)
    addi sp, sp, 80
    ret

// Default software vector table for system interrupts, routed through
// mip.meip. Note this is assumed in e.g. hardware_irq to begin exactly 0x34
// words after the hardware vector table indicated by mtvec (defined above).
.p2align 4
.global __soft_vector_table
__soft_vector_table:
.word isr_uart
.word isr_irq1
.word isr_irq2
.word isr_irq3

// all default trap handlers do nothing, and we can check for them being set to our
// default values by seeing if they point to somewhere between __default_isrs_start and __default_isrs_end
.global __default_isrs_start
__default_isrs_start:

// Declare a weak symbol for each ISR.
// By default, they will fall through to the undefined IRQ handler below (breakpoint),
// but can be overridden by C functions with correct name.

.macro decl_isr name
.weak \name
\name:
.endm

decl_isr isr_uart
decl_isr isr_irq1
decl_isr isr_irq2
decl_isr isr_irq3
// fall through

// All unhandled USER IRQs fall through to here. Note there is no way to get
// the "current exception" on RISC-V (as there is no such thing -- the
// hardware does not model the exception lifecycle like on Arm) so instead we
// just pass the IRQ number into the IRQ handler in a0.
.global __unhandled_user_irq
__unhandled_user_irq:
.global unhandled_user_irq_num_in_a0
unhandled_user_irq_num_in_a0:
    // The dispatch code will have left IRQ << 2 in a0 from its meinext read,
    // so just need to shift it back down
    srli a0, a0, 2
    ebreak
1:
    j 1b

.global __default_isrs_end
__default_isrs_end:
