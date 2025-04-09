#include <stdint.h>
#include <stdbool.h>

#include "dma_drv.h"

void dma_init(DMA dma, dma_config_t *cfg)
{
    dma->CTRL = cfg->data_width << DMA_CTRL_DWIDTH_LSB |
                cfg->src_addr_auto_inc << DMA_CTRL_SADDRINC_LSB |
                cfg->dest_addr_auto_inc << DMA_CTRL_DADDRINC_LSB |
                cfg->tran_over_irq_en << DMA_CTRL_OVER_IRQ_LSB |
                cfg->tran_half_irq_en << DMA_CTRL_HALF_IRQ_LSB |
                cfg->count_mode << DMA_CTRL_COUNT_MODE_LSB;

    dma->SDADDR = cfg->src_data_addr;
    dma->SSADDR = cfg->src_status_addr;
    dma->SRBIT = cfg->src_ready_bit;
    dma->SRPOL = cfg->src_ready_polarity;
    dma->DDADDR = cfg->dest_data_addr;
    dma->DSADDR = cfg->dest_status_addr;
    dma->DRBIT = cfg->dest_ready_bit;
    dma->DRPOL = cfg->dest_ready_polarity;
    dma->COUNT = cfg->count;
}

void dma_start(DMA dma, uint8_t en)
{
    if (en)
        dma->CTRL |= 1 << DMA_CTRL_START_LSB;
    else
        dma->CTRL &= ~(1 << DMA_CTRL_START_LSB);
}

uint8_t dma_transform_completed(DMA dma)
{
    return (dma->IRQ_PEND & (1 << DMA_IRQ_PEND_OVER_PEND_LSB));
}

uint8_t dma_transform_half_completed(DMA dma)
{
    return (dma->IRQ_PEND & (1 << DMA_IRQ_PEND_HALFOVER_PEND_LSB));
}

void dma_clear_over_irq_pending(DMA dma)
{
    dma->IRQ_PEND = 1 << DMA_IRQ_PEND_OVER_PEND_LSB;
}

void dma_clear_halfover_irq_pending(DMA dma)
{
    dma->IRQ_PEND = 1 << DMA_IRQ_PEND_HALFOVER_PEND_LSB;
}
