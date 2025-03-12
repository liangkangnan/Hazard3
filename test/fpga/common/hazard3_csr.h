#ifndef _HAZARD3_CSR_H
#define _HAZARD3_CSR_H

#ifndef __ASSEMBLER__
#include "stdint.h"
#endif

#define hazard3_csr_mstatus    0x300
#define hazard3_csr_mie        0x304
#define hazard3_csr_mtvec      0x305 // Trap vector base address
#define hazard3_csr_mscratch   0x340
#define hazard3_csr_mepc       0x341
#define hazard3_csr_mcause     0x342

#define hazard3_csr_dmdata0    0xbff // Debug-mode shadow CSR for DM data transfer

#define hazard3_csr_meiea      0xbe0 // External interrupt pending array
#define hazard3_csr_meipa      0xbe1 // External interrupt enable array
#define hazard3_csr_meifa      0xbe2 // External interrupt force array
#define hazard3_csr_meipra     0xbe3 // External interrupt priority array
#define hazard3_csr_meinext    0xbe4 // Next external interrupt
#define hazard3_csr_meicontext 0xbe5 // External interrupt context register

#define hazard3_csr_mswint     0xbe9 // M-mode software interrupt register
#define hazard3_csr_mspbottom  0xbea // M-mode sp bottom register
#define hazard3_csr_msleep     0xbf0 // M-mode sleep control register

#define _read_csr(csrname) ({ \
    uint32_t __csr_tmp_u32; \
    asm volatile ("csrr %0, " #csrname : "=r" (__csr_tmp_u32)); \
    __csr_tmp_u32; \
})

#define _write_csr(csrname, data) ({ \
    if (__builtin_constant_p(data) && !((data) & -32u)) { \
        asm volatile ("csrwi " #csrname ", %0" : : "i" (data)); \
    } else { \
        asm volatile ("csrw " #csrname ", %0" : : "r" (data)); \
    } \
})

#define _set_csr(csrname, data) ({ \
    if (__builtin_constant_p(data) && !((data) & -32u)) { \
        asm volatile ("csrsi " #csrname ", %0" : : "i" (data)); \
    } else { \
        asm volatile ("csrs " #csrname ", %0" : : "r" (data)); \
    } \
})

#define _clear_csr(csrname, data) ({ \
    if (__builtin_constant_p(data) && !((data) & -32u)) { \
        asm volatile ("csrci " #csrname ", %0" : : "i" (data)); \
    } else { \
        asm volatile ("csrc " #csrname ", %0" : : "r" (data)); \
    } \
})

#define _read_write_csr(csrname, data) ({ \
    uint32_t __csr_tmp_u32; \
    if (__builtin_constant_p(data) && !((data) & -32u)) { \
        asm volatile ("csrrwi %0, " #csrname ", %1": "=r" (__csr_tmp_u32) : "i" (data)); \
    } else { \
        asm volatile ("csrrw %0, " #csrname ", %1": "=r" (__csr_tmp_u32) : "r" (data)); \
    } \
    __csr_tmp_u32; \
})

#define _read_set_csr(csrname, data) ({ \
    uint32_t __csr_tmp_u32; \
    if (__builtin_constant_p(data) && !((data) & -32u)) { \
        asm volatile ("csrrsi %0, " #csrname ", %1": "=r" (__csr_tmp_u32) : "i" (data)); \
    } else { \
        asm volatile ("csrrs %0, " #csrname ", %1": "=r" (__csr_tmp_u32) : "r" (data)); \
    } \
    __csr_tmp_u32; \
})

#define _read_clear_csr(csrname, data) ({ \
    uint32_t __csr_tmp_u32; \
    if (__builtin_constant_p(data) && !((data) & -32u)) { \
        asm volatile ("csrrci %0, " #csrname ", %1": "=r" (__csr_tmp_u32) : "i" (data)); \
    } else { \
        asm volatile ("csrrc %0, " #csrname ", %1": "=r" (__csr_tmp_u32) : "r" (data)); \
    } \
    __csr_tmp_u32; \
})

// Argument macro expansion layer (CSR name may be a macro that expands to a
// CSR number, or it may be a bare name that the assembler knows about.)
#define read_csr(csrname)             _read_csr(csrname)
#define write_csr(csrname, data)      _write_csr(csrname, data)
#define set_csr(csrname, data)        _set_csr(csrname, data)
#define clear_csr(csrname, data)      _clear_csr(csrname, data)
#define read_write_csr(csrname, data) _read_write_csr(csrname, data)
#define read_set_csr(csrname, data)   _read_set_csr(csrname, data)
#define read_clear_csr(csrname, data) _read_clear_csr(csrname, data)

#endif
