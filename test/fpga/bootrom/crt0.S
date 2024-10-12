#define APP_START_ADDR  0x100
#define APP_LEN_ADDR    (0x100 - 0x4)
#define IRAM_ADDR       0x10000000

.section .text

// Hardware vector table for standard RISC-V interrupts.

j _isr_riscv_machine_exception
.word 0
.word 0
j _isr_riscv_machine_soft_irq
.word 0
.word 0
.word 0
j _isr_riscv_machine_timer
.word 0
.word 0
.word 0
j _isr_riscv_machine_external_irq
.word 0
.word 0
.word 0
.word 0
j _reset_handler

.weak _isr_riscv_machine_exception
.weak _isr_riscv_machine_soft_irq
.weak _isr_riscv_machine_timer
.weak _isr_riscv_machine_external_irq

_isr_riscv_machine_exception:
_isr_riscv_machine_soft_irq:
_isr_riscv_machine_timer:
_isr_riscv_machine_external_irq:
    ebreak
1:
    j 1b

// ----------------------------------------------------------------------------

// ELF entry point:
.global _entry_point
_entry_point:

// Reset handler
_reset_handler:

copy_data:
    /* Load user APP from flash to IRAM */
    li a0, APP_START_ADDR
    li a1, IRAM_ADDR
    li a2, APP_LEN_ADDR
    lw a2, (a2)
    li a3, 0x0
1:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    addi a3, a3, 4
    bltu a3, a2, 1b

jump_to_iram:
    li a0, IRAM_ADDR
    addi a0, a0, 0x40
    jalr a0, a0
    ebreak
1:
    j 1b
