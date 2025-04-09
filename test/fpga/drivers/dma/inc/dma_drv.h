#ifndef _DMA_DRV_H_
#define _DMA_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#include "dma_regs.h"

#define DMA0_BASE_ADDR  (0x4000b000)
#define DMA1_BASE_ADDR  (0x4000c000)

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t SDADDR;
    volatile uint32_t SSADDR;
    volatile uint32_t SRBIT;
    volatile uint32_t SRPOL;
    volatile uint32_t DDADDR;
    volatile uint32_t DSADDR;
    volatile uint32_t DRBIT;
    volatile uint32_t DRPOL;
    volatile uint32_t COUNT;
    volatile uint32_t IRQ_PEND;
} dma_hw_t;

#define dma0_hw ((dma_hw_t *)DMA0_BASE_ADDR)
#define dma1_hw ((dma_hw_t *)DMA1_BASE_ADDR)

typedef dma_hw_t *DMA;

#define dma0 dma0_hw
#define dma1 dma1_hw

#define DMA_DATA_WIDTH_8  0
#define DMA_DATA_WIDTH_16 1
#define DMA_DATA_WIDTH_32 2

#define DMA_STATUS_READY  32

#define DMA_COUNT_ONESHOT 0
#define DMA_COUNT_CIRCLE  1

typedef struct {
    uint8_t count_mode;
    uint8_t data_width;
    uint8_t src_addr_auto_inc;
    uint8_t dest_addr_auto_inc;
    uint8_t tran_over_irq_en;
    uint8_t tran_half_irq_en;
    uint32_t src_data_addr;
    uint32_t src_status_addr;
    uint8_t src_ready_bit;
    uint8_t src_ready_polarity;
    uint32_t dest_data_addr;
    uint32_t dest_status_addr;
    uint8_t dest_ready_bit;
    uint8_t dest_ready_polarity;
    uint32_t count;
} dma_config_t;

void dma_init(DMA dma, dma_config_t *cfg);
void dma_start(DMA dma, uint8_t en);
uint8_t dma_transform_completed(DMA dma);
uint8_t dma_transform_half_completed(DMA dma);
void dma_clear_over_irq_pending(DMA dma);
void dma_clear_halfover_irq_pending(DMA dma);

#endif
