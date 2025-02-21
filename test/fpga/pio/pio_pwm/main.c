#include <stdint.h>
#include <stdbool.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "pwm.h"
#include "delay.h"

PIO pio = pio0;
uint32_t sm = 0;

// 占空比：0%~100%，step=1%
void pio_pwm_set_duty(PIO pio, uint32_t sm, uint32_t duty)
{
    uint32_t low_count;
    uint32_t data[4];
    uint32_t *p;
    uint32_t i;
    uint32_t count;

    while (pio_sm_get_tx_fifo_shadow_update_state(pio, sm));

    if (duty > 100)
        duty = 100;

    low_count = 100 - duty;

    for (i = 0; i < 4; i++)
        data[i] = 0xffffffff;

    p = data;
    while (low_count > 0) {
        if (low_count >= 25) {
            count = 25;
            *p = 0x00;
        } else {
            count = low_count;
            for (i = 0; i < count; i++)
                *p &= ~(1 << i);
        }
        p++;
        low_count -= count;
    }

    pio_sm_rx_fifo_write_enable(pio, sm, true);
    pio_sm_set_rx_fifo_push_index(pio, sm, 0);
    for (i = 0; i < 4; i++) {
        pio_sm_put(pio, sm, data[i]);
        //printf("put data[%d]=0x%x\n", i, data[i]);
    }
    pio_sm_rx_fifo_write_enable(pio, sm, false);

    pio_sm_set_tx_fifo_shadow_update(pio, sm);
}

int main()
{
    uart_init(115200);

    printf("hello pio pwm!!!\n");

    pio_sm_config config = {0};
    pio_sm_config_set_out_pins(&config, 8, 1);
    pio_sm_config_set_out_shift(&config, true, true, 25);
    pio_sm_config_set_wrap(&config, 0, pwm_program.length - 1);
    pio_sm_config_set_clkdiv(&config, 120, 0);
    pio_sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_TX);
    pio_add_program_at_offset(pio, &pwm_program, 0);

    pio_sm_set_consecutive_pindirs(pio, sm, 8, 1, true);
    pio_sm_init(pio, sm, 0, &config);

    pio_sm_set_tx_fifo_shadow_mode_enabled(pio, sm, 1);

    // init duty: 0%
    pio_sm_clear_fifos(pio, sm);
	pio_sm_put(pio, sm, 0x00);
	pio_sm_put(pio, sm, 0x00);
	pio_sm_put(pio, sm, 0x00);
	pio_sm_put(pio, sm, 0x00);
    pio_sm_clear_fifos(pio, sm);

    pio_sm_set_enabled(pio, sm, true);

    printf("pio pwm started\n");

    while (1) {
        // 1KHz, 10%
        pio_pwm_set_duty(pio, sm, 10);
        delay_ms(500);
        // 1KHz, 90%
        pio_pwm_set_duty(pio, sm, 90);
        delay_ms(500);
    }
}
