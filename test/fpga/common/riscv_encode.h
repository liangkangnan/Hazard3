#ifndef _RISCV_ENCODE_H_
#define _RISCV_ENCODE_H_

// Helpers for encoding RISC-V immediates

// U format, e.g. lui
static inline uint32_t riscv_encode_imm_u(uint32_t x) {
    return (x >> 12) << 12;
}

// I format, e.g. addi
static inline uint32_t riscv_encode_imm_i(uint32_t x) {
    return (x & 0xfff) << 20;
}

// The U-format part of a U+I 32-bit immediate:
static inline uint32_t riscv_encode_imm_u_hi(uint32_t x) {
    // We will add a signed 12 bit constant to the "lui" value,
    // so we need to correct for the carry here.
    x += (x & 0x800) << 1;
    return riscv_encode_imm_u(x);
}

// B format, e.g. bgeu
static inline uint32_t riscv_encode_imm_b(uint32_t x) {
    return
        (((x >> 12) & 0x01) << 31) |
        (((x >>  5) & 0x3f) << 25) |
        (((x >>  1) & 0x0f) <<  8) |
        (((x >> 11) & 0x01) <<  7);
}

// S format, e.g. sw
static inline uint32_t riscv_encode_imm_s(uint32_t x) {
    return
        (((x >>  5) & 0x7f) << 25) |
        (((x >>  0) & 0x1f) <<  7);
}

// J format, e.g. jal
static inline uint32_t riscv_encode_imm_j(uint32_t x) {
    return
        (((x >> 20) & 0x001) << 31) |
        (((x >>  1) & 0x3ff) << 21) |
        (((x >> 11) & 0x001) << 20) |
        (((x >> 12) & 0x0ff) << 12);
}

// CJ format, e.g. c.jal
static inline uint16_t riscv_encode_imm_cj(uint32_t x) {
    return (uint16_t)(
        (((x >> 11) & 0x1) << 12) |
        (((x >>  4) & 0x1) << 11) |
        (((x >>  8) & 0x3) <<  9) |
        (((x >> 10) & 0x1) <<  8) |
        (((x >>  6) & 0x1) <<  7) |
        (((x >>  7) & 0x1) <<  6) |
        (((x >>  1) & 0x7) <<  3) |
        (((x >>  5) & 0x1) <<  2)
    );
}

// CB format, e.g. c.beqz
static inline uint16_t riscv_encode_imm_cb(uint32_t x) {
    return (uint16_t)(
        (((x >> 8) & 0x1) << 12) |
        (((x >> 3) & 0x3) << 10) |
        (((x >> 6) & 0x3) <<  5) |
        (((x >> 1) & 0x3) <<  3) |
        (((x >> 5) & 0x1) <<  2)
    );
}

// CI format, e.g. c.addi
static inline uint16_t riscv_encode_imm_ci(uint32_t x) {
    return (uint16_t)(
        (((x >> 5) & 0x01) << 12) |
        (((x >> 0) & 0x1f) <<  2)
    );
}

#endif
