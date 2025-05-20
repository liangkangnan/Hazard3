#ifndef _CRC_DRV_H_
#define _CRC_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#include "crc_regs.h"

#define CRC_BASE_ADDR  (0x4000e000)


typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t STAT;
    volatile uint32_t INIT;
    volatile uint32_t POLY;
    volatile uint32_t RESULT;
    volatile uint32_t DATA;
} crc_t;

#define CRC ((crc_t *)CRC_BASE_ADDR)


static inline void crc_enable(uint8_t en)
{
    CRC->CTRL = en;
}

static inline uint8_t crc_is_idle()
{
    if (CRC->STAT & (1 << CRC_STAT_IDLE_LSB))
        return 1;
    else
        return 0;
}

static inline uint8_t crc_is_full()
{
    if (CRC->STAT & (1 << CRC_STAT_FIFO_FULL_LSB))
        return 1;
    else
        return 0;
}

static inline uint8_t crc_is_empty()
{
    if (CRC->STAT & (1 << CRC_STAT_FIFO_EMPTY_LSB))
        return 1;
    else
        return 0;
}

static inline void crc_set_init_val(uint32_t val)
{
    CRC->INIT = val;
}

static inline void crc_set_poly_val(uint32_t val)
{
    CRC->POLY = val;
}

static inline void crc_put_data(uint8_t val)
{
    CRC->DATA = val;
}

static inline void crc_put_data_blocking(uint8_t val)
{
    while (CRC->STAT & (1 << CRC_STAT_FIFO_FULL_LSB));
    CRC->DATA = val;
}

static inline uint32_t crc_get_result()
{
    return CRC->RESULT;
}

#endif
