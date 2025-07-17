#ifndef _TFPU_DRV_H_
#define _TFPU_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#include "tfpu_regs.h"

#define TFPU_BASE_ADDR  (0x40007000)


typedef struct {
    volatile uint32_t CSR;
    volatile uint32_t OP_A;
    volatile uint32_t OP_B;
    volatile uint32_t RES_Z;
} tfpu_t;

#define TFPU ((tfpu_t *)TFPU_BASE_ADDR)

typedef enum {
    TFPU_ADD = 0,
    TFPU_MUL = 1,
    TFPU_DIV = 2,
    TFPU_SIN = 3,
    TFPU_COS = 4,
    TFPU_SQRT = 5,
} tfpu_op_e;

// IEEE 754单精度浮点数转换函数
typedef union {
    float f;
    uint32_t u;
    struct {
        uint32_t mantissa : 23;
        uint32_t exponent : 8;
        uint32_t sign : 1;
    } parts;
} float_union_t;

static inline float tfpu_arith(float a, float b, tfpu_op_e op)
{
    float_union_t res;

    TFPU->OP_A = *((int *)&a);
    TFPU->OP_B = *((int *)&b);
    TFPU->CSR = (op << TFPU_CSR_OP_LSB);
    TFPU->CSR |= (1 << TFPU_CSR_START_LSB);
    while (TFPU->CSR & (1 << TFPU_CSR_START_LSB));
    res.u = TFPU->RES_Z;
    return res.f;
}

static inline float tfpu_cordic(float a, tfpu_op_e op)
{
    a = a * 16384.0f + 0.5f;
    TFPU->OP_A = (int)a;
    TFPU->CSR = (op << TFPU_CSR_OP_LSB);
    TFPU->CSR |= (1 << TFPU_CSR_START_LSB);
    while (TFPU->CSR & (1 << TFPU_CSR_START_LSB));
    return (float)(((int16_t)(TFPU->RES_Z & 0xffff)) / 16384.0f);
}

static inline uint32_t tfpu_sqrt(uint32_t a)
{
    TFPU->OP_A = a;
    TFPU->CSR = (TFPU_SQRT << TFPU_CSR_OP_LSB);
    TFPU->CSR |= (1 << TFPU_CSR_START_LSB);
    while (TFPU->CSR & (1 << TFPU_CSR_START_LSB));
    return TFPU->RES_Z;
}

#endif
