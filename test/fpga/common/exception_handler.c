#include <stdint.h>
#include <stdbool.h>

#include "printf.h"
#include "uart_drv.h"
#include "hazard3_csr.h"

static void printf_exception_info()
{
    uart_init(115200);

    printf("----------------------------------------------\n");
    printf("exception, mcause = %u\n", read_csr(mcause));

    uint32_t mepc = read_csr(mepc);

    printf("exception pc: 0x%08x\n", mepc);
    if ((*(uint16_t*)mepc & 0x3) == 0x3) {
        printf("exception instr: %04x%04x\n", *(uint16_t*)(mepc + 2), *(uint16_t*)mepc);
    } else {
        printf("exception instr: %04x\n", *(uint16_t*)mepc);
    }

    printf("**********************************************\n");
}

void isr_riscv_machine_instr_align_exception()
{
    printf_exception_info();
    while (1);
}

void isr_riscv_machine_instr_fault_exception()
{
    printf_exception_info();
    while (1);
}

void isr_riscv_machine_instr_illegal_exception()
{
    printf_exception_info();
    while (1);
}

void isr_riscv_machine_load_align_exception()
{
    printf_exception_info();
    while (1);
}

void isr_riscv_machine_load_fault_exception()
{
    printf_exception_info();
    while (1);
}

void isr_riscv_machine_store_align_exception()
{
    printf_exception_info();
    while (1);
}

void isr_riscv_machine_store_fault_exception()
{
    printf_exception_info();
    while (1);
}

void isr_riscv_machine_ebreak_exception()
{
    printf_exception_info();
    printf("%s\n", __func__);
    while (1);
}

void isr_riscv_machine_ecall_umode_exception()
{
    printf_exception_info();
    printf("%s\n", __func__);
    while (1);
}

void isr_riscv_machine_ecall_smode_exception()
{
    printf_exception_info();
    printf("%s\n", __func__);
    while (1);
}

void isr_riscv_machine_ecall_mmode_exception()
{
    printf_exception_info();
    printf("%s\n", __func__);
    while (1);
}
