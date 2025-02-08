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

// 占空比：3%~97%，step=1%
void pio_pwm_set_duty(PIO pio, uint32_t sm, uint32_t low_count, uint32_t high_count)
{
    static uint32_t low_count_prev;
    static uint32_t high_count_prev;
    static uint8_t first_update = 1;

    while (pio_sm_get_tx_fifo_shadow_update_state(pio, sm));

    // PIO程序里会多2条指令+1次循环，因此需要减3
    low_count -= 3;
    high_count -= 3;

    if (first_update) {
        pio_sm_put(pio, sm, low_count);
        pio_sm_put(pio, sm, high_count);
        first_update = 0;
    } else {
        pio_sm_put(pio, sm, low_count_prev);
        pio_sm_put(pio, sm, high_count_prev);
    }
    pio_sm_put(pio, sm, low_count);
    pio_sm_put(pio, sm, high_count);

    pio_sm_set_tx_fifo_shadow_update(pio, sm);

    low_count_prev = low_count;
    high_count_prev = high_count;
}

int main()
{
    uart_init(115200);

    printf("hello pio pwm!!!\n");

    pio_sm_set_enabled(pio, sm, false);

    pio_sm_config config;
    pio_sm_config_set_sideset(&config, 8, 1, true, false);
    pio_sm_config_set_wrap(&config, 0, pwm_program.length - 1);
    pio_sm_config_set_clkdiv(&config, 120, 0);
    pio_add_program_at_offset(pio, &pwm_program, 0);

    pio_sm_set_consecutive_pindirs(pio, sm, 8, 1, true);
    pio_sm_init(pio, sm, 0, &config);

    pio_sm_set_tx_fifo_peek_mode_enabled(pio, sm, 1);
    pio_sm_set_tx_fifo_shadow_mode_enabled(pio, sm, 1);

    printf("pio pwm started\n");

    pio_pwm_set_duty(pio, sm, 80, 20);

    pio_sm_set_enabled(pio, sm, true);

    printf("end\n");

    while (1) {
        // 1KHz, 20%
        pio_pwm_set_duty(pio, sm, 80, 20);
        delay_ms(500);
        // 1KHz, 70%
        pio_pwm_set_duty(pio, sm, 30, 70);
        delay_ms(500);
    }
}
