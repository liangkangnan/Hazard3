/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RISCV_SYNC_H
#define _RISCV_SYNC_H

#include "hazard3_csr.h"
#include "hazard3_instr.h"

#ifndef __always_inline
#define __always_inline __attribute__((__always_inline__))
#endif

#define __force_inline inline __always_inline

#ifdef __cplusplus
extern "C" {
#endif

__force_inline static void __nop(void) {
    asm volatile ("nop");
}

#if !__has_builtin(__sev)
__force_inline static void __sev(void) {
    __hazard3_unblock();
}
#endif

#if !__has_builtin(__wfe)
__force_inline static void __wfe(void) {
    __hazard3_block();
}
#endif

#if !__has_builtin(__wfi)
__force_inline static void __wfi(void) {
    asm volatile("wfi");
}
#endif

__force_inline static void __dmb(void) {
    asm volatile ("fence rw, rw" : : : "memory");
}

__force_inline static void __dsb(void) {
    asm volatile ("fence rw, rw" : : : "memory");
}

__force_inline static void __isb(void) {
    asm volatile ("fence.i" : : : "memory");
}

__force_inline static void __mem_fence_acquire(void) {
    __dmb();
}

__force_inline static void __mem_fence_release(void) {
    __dmb();
}

__force_inline static uint32_t save_and_disable_interrupts(void) {
    uint32_t status;
    asm volatile (
        "csrrci %0, mstatus, 0x8\n"
        : "=r" (status) :: "memory"
    );
    return status;
}

__force_inline static void __compiler_memory_barrier(void) {
    asm volatile ("" : : : "memory");
}

__force_inline static void restore_interrupts(uint32_t status) {
    __compiler_memory_barrier();
    if (status & 0x8) {
        set_csr(mstatus, 8);
    } else {
        clear_csr(mstatus, 8);
    }
    __compiler_memory_barrier();
}

__force_inline static void restore_interrupts_from_disabled(uint32_t status) {
    __compiler_memory_barrier();
    set_csr(mstatus, status & 8);
    __compiler_memory_barrier();
}

#ifdef __cplusplus
}
#endif

#endif
