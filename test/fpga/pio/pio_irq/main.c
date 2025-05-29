#include <stdint.h>
#include <stdbool.h>

#include "hazard3_irq.h"
#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "irq.h"

#define PIO0_IRQ_NUM         1
#define PIO0_IRQ_PRIORITY    4

PIO pio;
uint32_t sm;

void pio0_irq_handler();

int main()
{
    uart_init(115200);

    printf("hello pio irq!!!\n");

    global_irq_enable(true);
    external_irq_enable(true);
    h3irq_enable(PIO0_IRQ_NUM, true);
    h3irq_set_priority(PIO0_IRQ_NUM, PIO0_IRQ_PRIORITY);
    h3irq_set_external_irq_handler(PIO0_IRQ_NUM, pio0_irq_handler);

    int offset = pio_add_program(&pio, &sm, &irq_program);
    if (offset < 0) {
        printf("Not enough space for sm!\n");
        return -1;
    }

    pio_sm_config config = {0};
    pio_sm_config_set_wrap(&config, offset + irq_wrap_bottom, offset + irq_wrap_top);
    pio_sm_config_set_instr_offset(&config, offset);
    pio_sm_config_set_clkdiv(&config, 120000, 0);
    pio_sm_config_set_out_pins(&config, 0, 1);
    pio_sm_config_set_out_shift(&config, true, false, 32);
    pio_sm_init(pio, sm, 0, &config);

    pio_sm_set_consecutive_pindirs(pio, sm, 0, 1, true);

    pio_set_irq_source_enabled(pio, pio_sm_irq0, true);

    pio_sm_set_enabled(pio, sm, true);

    printf("pio irq started\n");

    // trigger sm0 tx empty irq.
    pio_set_irq_source_enabled(pio, pio_sm0_tx_fifo_empty_irq, true);

    while (1) {

    }
}

void pio0_irq_handler()
{
    if (pio_interrupt_get(pio, pio_sm_irq0)) {
        pio_interrupt_clear(pio, pio_sm_irq0);
        printf("get sm irq 0.\n");
    }
    if (pio_interrupt_get(pio, pio_sm0_tx_fifo_empty_irq)) {
        printf("get sm0 tx empty irq.\n");
        pio_sm_put(pio, sm, 0xff);
        pio_interrupt_clear(pio, pio_sm0_tx_fifo_empty_irq);
    }
}
