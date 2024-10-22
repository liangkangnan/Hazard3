#define RVCSR_MEICONTEXT_OFFSET         (0x00000be5)
#define RVCSR_MEICONTEXT_CLEARTS_BITS   (0x00000002)
#define RVCSR_MEINEXT_OFFSET            (0x00000be4)
#define RVCSR_MEINEXT_UPDATE_BITS       (0x00000001)
#define RVCSR_MEIFA_OFFSET              (0x00000be2)
#define RVCSR_MIE_MEIE_BITS             (0x00000800)
#define RVCSR_MSTATUS_MIE_BITS          (0x00000008)
#define RVCSR_MSPBOTTOM_OFFSET          (0x00000bea)

.section .vectors, "ax"

.p2align 6
.global __vectors, __VECTOR_TABLE
__VECTOR_TABLE:
__vectors:

// Hardware vector table for standard RISC-V interrupts, indicated by `mtvec`.

.option push
.option norvc
.option norelax
j _isr_riscv_machine_exception
.word 0
.word 0
j isr_riscv_machine_soft_irq
.word 0
.word 0
.word 0
j isr_riscv_machine_timer
.word 0
.word 0
.word 0
j _isr_riscv_machine_external_irq
.word 0
.word 0
.word 0
.word 0
j _reset_handler
.option pop

.weak isr_riscv_machine_soft_irq
isr_riscv_machine_soft_irq:
    ebreak
1:
    j 1b

.weak isr_riscv_machine_timer
isr_riscv_machine_timer:
    ebreak
1:
    j 1b

_isr_riscv_machine_exception:
    addi sp, sp, -8
    sw ra,  4(sp)
    la ra, isr_riscv_machine_exception
    jalr ra, ra
    lw ra,  4(sp)
    addi sp, sp, 8
    mret

_isr_riscv_machine_external_irq:
    addi sp, sp, -8
    sw ra,  4(sp)
    la ra, isr_riscv_machine_external_irq
    jalr ra, ra
    lw ra,  4(sp)
    addi sp, sp, 8
    mret

// ----------------------------------------------------------------------------

.section .reset, "ax"

// ELF entry point:
.global _entry_point
_entry_point:

// Reset handler:
// - initialises .data
// - clears .bss
// - calls runtime_init
// - calls main

_reset_handler:
.option push
.option norelax
    la gp, __global_pointer$
.option pop
    la sp, _stack_top
    // Enable stack overflow detect
    la a0, _stack_bottom
    addi a0, a0, 1
    csrw RVCSR_MSPBOTTOM_OFFSET, a0
    // Leave interrupts globally disabled for now, we will set that up later
    // in runtime_init_per_core_h3_irq_registers. Still worth installing the vector table:
    la a0, __vectors + 1
    csrw mtvec, a0
    csrci mcountinhibit, 0x5

copy_data:
	/* Load data section from flash to RAM */
	la a0, _data_lma
	la a1, _data_vma
	la a2, _edata
	bgeu a1, a2, clear_bss
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b

clear_bss:
	/* Clear bss section */
	la a0, _sbss
	la a1, _ebss
	bgeu a0, a1, platform_entry
1:
	sw zero, (a0)
	addi a0, a0, 4
	bltu a0, a1, 1b

platform_entry: // symbol for stack traces
    // Use `call` pseudo-instruction instead of a bare `jal` so that the
    // linker can use longer sequences if these are out of `jal` range. Will
    // still get relaxed to a `jal` if possible.
    // call runtime_init_per_core_h3_irq_registers (now called in runtime_init)
    la ra, qspi_init
    jalr ra, ra
    call runtime_init
    call main
    ebreak
1:
    j 1b

// ----------------------------------------------------------------------------
// Provide safe defaults for _exit and runtime_init
// Full implementations usually provided by platform.c

.weak runtime_init
runtime_init:
    ret

// ----------------------------------------------------------------------------
// IRQ register clearing

// Enable interrupts globally, but disable all interrupt sources.
//
// This is put in the .text section as it is called again on core 1 launch. In
// COPY_TO_RAM binaries, the .reset section is always in flash, whereas .text
// is in SRAM, and we try to avoid running any code from flash after entering
// user code in a COPY_TO_RAM binary. Note because of this we don't call this
// function here in crt0 until after the flash-to-RAM copying is finished.

.section .text
.global runtime_init_per_core_h3_irq_registers
runtime_init_per_core_h3_irq_registers:
    // First clear all IRQ force array bits. Iterate over array registers 0
    // through 3 inclusive, allowing for up to 64 IRQs. Don't clear the
    // enable array as earlier (non-per-core) init stages may have already
    // set up IRQs.
    li a0, 3
1:
    csrw RVCSR_MEIFA_OFFSET, a0
    addi a0, a0, -1
    bgez a0, 1b
    // Setting the global external IRQ enable in mie prepares us to enable
    // IRQs one-by-one later. Also clear the soft IRQ and timer IRQ enables:
    li a0, RVCSR_MIE_MEIE_BITS
    csrw mie, a0
    // Set the global IRQ: we will now take any individual interrupt that is
    // pending && enabled
    csrsi mstatus, RVCSR_MSTATUS_MIE_BITS
    // Take this chance to clear mscratch, which is used to detect nested
    // exceptions in isr_riscv_machine_exception:
    csrw mscratch, zero
    ret

// ----------------------------------------------------------------------------
// Enable QSPI Mode

#define FLASH_CSR  (0x40008000)
#define FLASH_TX   (0x40008004)
#define FLASH_RX   (0x40008008)
#define FLASH_QSPI (0x4000800C)

.section .qspi.init.inram

__flash_ctrl_busy:
    li a0, FLASH_CSR
    lw a1, (a0)
    andi a1, a1, 0x2
    bgtz a1, __flash_ctrl_busy
    ret

qspi_init:
    mv t0, ra
    jal write_enable
    jal enable_flash_qspi
    jal enable_xip_qspi
    mv ra, t0
    ret

write_enable:
    mv t1, ra

    // enable direct mode
    li a0, FLASH_CSR
    li a1, 0x1
    sw a1, (a0)

    // write enable
    li a0, FLASH_TX
    li a1, 0x06
    sw a1, (a0)
    jal __flash_ctrl_busy

    // disable direct mode
    li a0, FLASH_CSR
    li a1, 0x0
    sw a1, (a0)

    mv ra, t1
    ret

enable_flash_qspi:
    mv t1, ra

    // enable direct mode
    li a0, FLASH_CSR
    li a1, 0x1
    sw a1, (a0)

    // write cmd 0x01
    li a0, FLASH_TX
    li a1, 0x01
    sw a1, (a0)
    jal __flash_ctrl_busy

    // write data 0x00
    li a0, FLASH_TX
    li a1, 0x00
    sw a1, (a0)
    jal __flash_ctrl_busy

    // write data 0x02
    li a0, FLASH_TX
    li a1, 0x02
    sw a1, (a0)
    jal __flash_ctrl_busy

    // disable direct mode
    li a0, FLASH_CSR
    li a1, 0x0
    sw a1, (a0)

    mv ra, t1
    ret

enable_xip_qspi:
    li a0, FLASH_QSPI
    // 4 dummy, enable qspi mode
    li a1, 0x9
    sw a1, (a0)
    ret
