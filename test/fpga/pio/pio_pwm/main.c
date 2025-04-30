#include <stdint.h>
#include <stdbool.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "pwm.h"
#include "delay.h"


#define PWM_OUT_IO  8

PIO pio;
uint32_t sm;

// 占空比：0%~100%，step=1%
void pio_pwm_set_duty(PIO pio, uint32_t sm, uint32_t duty)
{
    uint32_t low_count;
    uint32_t data[PIO_SM_FIFO_COUNT];
    uint32_t *p;
    uint32_t i;
    uint32_t count;

    while (pio_sm_get_tx_fifo_shadow_update_state(pio, sm));

    if (duty > 100)
        duty = 100;

    low_count = (100 - duty) * 256 / 100;

    for (i = 0; i < PIO_SM_FIFO_COUNT; i++)
        data[i] = 0x00;

    p = data;
    while (low_count > 0) {
        if (low_count >= 32) {
            count = 32;
            *p = 0xffffffff;
        } else {
            count = low_count;
            for (i = 0; i < count; i++)
                *p |= (1 << i);
        }
        p++;
        low_count -= count;
    }

    pio_sm_rx_fifo_write_enable(pio, sm, true);
    pio_sm_set_rx_fifo_push_index(pio, sm, 0);
    for (i = 0; i < PIO_SM_FIFO_COUNT; i++) {
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

    int offset = pio_add_program(&pio, &sm, &pwm_program);
    if (offset < 0) {
        printf("Not enough space for sm!\n");
        return -1;
    }

    pio_sm_config config = {0};
    pio_sm_config_set_out_pins(&config, PWM_OUT_IO, 1);
    pio_sm_config_set_out_shift(&config, true, true, 32);
    pio_sm_config_set_wrap(&config, offset + pwm_wrap_bottom, offset + pwm_wrap_top);
    // pwm freq = 12M / 4 / 256 = 11.7KHz
    pio_sm_config_set_clkdiv(&config, 4, 0);
    pio_sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_TX);
    pio_add_program_at_offset(pio, &pwm_program, 0);

    pio_sm_set_consecutive_pindirs(pio, sm, PWM_OUT_IO, 1, true);
    pio_sm_init(pio, sm, 0, &config);

    pio_sm_set_tx_fifo_shadow_mode_enabled(pio, sm, 1);

    // init duty: 0%
    pio_sm_clear_fifos(pio, sm);
    for (uint8_t i = 0; i < PIO_SM_FIFO_COUNT; i++)
        pio_sm_put(pio, sm, 0x00);
    pio_sm_clear_fifos(pio, sm);

    pio_sm_set_enabled(pio, sm, true);

    printf("pio pwm started\n");

    while (1) {
        // 11.7KHz, 10%
        pio_pwm_set_duty(pio, sm, 10);
        delay_ms(500);
        // 11.7KHz, 90%
        pio_pwm_set_duty(pio, sm, 90);
        delay_ms(500);
    }
}
