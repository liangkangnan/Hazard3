#include <stdint.h>

#include "uart_drv.h"
#include "dma_drv.h"
#include "printf.h"
#include "hazard3_irq.h"


#define DATA_NUM    10

static uint32_t src_data[DATA_NUM];
static uint32_t dest_data[DATA_NUM];

static volatile uint8_t dma_transform_over;

void dma0_irq_handler();

static void test_mem_to_mem()
{
    printf("### test_mem_to_mem ###\n");

    for (int i = 0; i < DATA_NUM; i++)
        src_data[i] = i;

    printf("before...\n");
    for (int i = 0; i < DATA_NUM; i++)
        printf("dest[%d]=0x%x\n", i, dest_data[i]);

    global_irq_enable(true);
    external_irq_enable(true);
    h3irq_enable(DMA0_IRQ_NUM, true);
    h3irq_set_priority(DMA0_IRQ_NUM, 4);
    h3irq_set_external_irq_handler(DMA0_IRQ_NUM, dma0_irq_handler);

    dma_config_t config;
    config.count_mode = DMA_COUNT_ONESHOT;
    config.data_width = DMA_DATA_WIDTH_32;
    config.src_addr_auto_inc = 1;
    config.dest_addr_auto_inc = 1;
    config.tran_over_irq_en = 1;
    config.tran_half_irq_en = 0;
    config.src_data_addr = (uint32_t)src_data;
    config.src_status_addr = 0;
    config.src_ready_bit = DMA_STATUS_READY;
    config.dest_data_addr = (uint32_t)dest_data;
    config.dest_status_addr = 0;
    config.dest_ready_bit = DMA_STATUS_READY;
    config.count = DATA_NUM * 4;
    dma_init(dma0, &config);

    dma_start(dma0, 1);

    while (!dma_transform_over);

    printf("complete\n");

    printf("after...\n");
    for (int i = 0; i < DATA_NUM; i++)
        printf("dest[%d]=0x%x\n", i, dest_data[i]);
}

static void test_mem_to_peri()
{
    printf("### test_mem_to_peri ###\n");

    for (int i = 0; i < DATA_NUM - 1; i++)
        src_data[i] = '0' + i;
    src_data[DATA_NUM - 1] = '\n';

    dma_config_t config;
    config.count_mode = DMA_COUNT_CIRCLE;
    config.data_width = DMA_DATA_WIDTH_32;
    config.src_addr_auto_inc = 1;
    config.dest_addr_auto_inc = 0;
    config.tran_over_irq_en = 0;
    config.tran_half_irq_en = 0;
    config.src_data_addr = (uint32_t)src_data;
    config.src_status_addr = 0;
    config.src_ready_bit = DMA_STATUS_READY;
    config.dest_data_addr = (uint32_t)(UART_BASE_ADDR + 0xc);
    config.dest_status_addr = (uint32_t)(UART_BASE_ADDR + 0x8);
    config.dest_ready_bit = UART_FSTAT_TXFULL_LSB;
    config.dest_ready_polarity = 0;
    config.count = DATA_NUM * 4;
    dma_init(dma1, &config);

    dma_start(dma1, 1);
}


int main()
{
    uart_init(115200);

    printf("hello dma!!!\n");

    test_mem_to_mem();
    test_mem_to_peri();

    while (1);
}

void dma0_irq_handler()
{
    dma_transform_over = 1;

    dma_clear_over_irq_pending(dma0);
}
