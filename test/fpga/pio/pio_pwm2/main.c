#include <stdint.h>
#include <stdbool.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "pwm2.h"
#include "delay.h"


#define PWM_OUT_IO  8

PIO pio;
uint32_t sm;

// 占空比：0%~100%，step=1%
void pio_pwm_set_duty(PIO pio, uint32_t sm, uint32_t duty)
{
    uint32_t low_count;
    uint32_t data[PIO_SM_FIFO_COUNT / 2];
    uint32_t *p;
    uint32_t i;
    uint32_t count;

    if (duty == 100) {
        pio_sm_set_enabled(pio, sm, false);
        pio_sm_exec(pio, sm, pio_encode_nop() | pio_encode_sideset(1, 1));
        return;
    } else if (duty == 0) {
        pio_sm_set_enabled(pio, sm, false);
        pio_sm_exec(pio, sm, pio_encode_nop() | pio_encode_sideset(1, 0));
        return;
    }

    while (pio_sm_get_tx_fifo_shadow_update_state(pio, sm));

    if (duty > 100)
        duty = 100;

    low_count = 300 - duty * 3 - 3;
    duty = duty * 3 - 3;

    data[0] = duty;
    data[1] = low_count;
    data[2] = duty;
    data[3] = low_count;

    pio_sm_set_tx_fifo_push_index(pio, sm, 0x4);
    for (i = 0; i < PIO_SM_FIFO_COUNT / 2; i++) {
        pio_sm_put(pio, sm, data[i]);
    }

    pio_sm_set_tx_fifo_shadow_update(pio, sm);

    if (!pio_sm_is_enabled(pio, sm)) {
        pio_sm_clear_tx_fifo(pio, sm);
        for (i = 0; i < PIO_SM_FIFO_COUNT / 2; i++) {
            pio_sm_put(pio, sm, data[i]);
        }
        pio_sm_set_enabled(pio, sm, true);
    }
}

int main()
{
    uart_init(115200);

    printf("hello pio pwm2!!!\n");

    int offset = pio_add_program(&pio, &sm, &pwm2_program);
    if (offset < 0) {
        printf("Not enough space for sm!\n");
        return -1;
    }

    pio_sm_config config = {0};
    pio_sm_config_set_sideset(&config, PWM_OUT_IO, 1, true, false);
    pio_sm_config_set_wrap(&config, offset + pwm2_wrap_bottom, offset + pwm2_wrap_top);
    pio_sm_config_set_instr_offset(&config, offset);
    // pwm freq = 12M / 2 / 300 = 20KHz
    pio_sm_config_set_clkdiv(&config, 2, 0);

    pio_sm_set_consecutive_pindirs(pio, sm, PWM_OUT_IO, 1, true);
    pio_sm_init(pio, sm, 0, &config);

    pio_sm_set_tx_fifo_shadow_mode_enabled(pio, sm, 1);

    printf("pio pwm2 started\n");

    while (1) {
        // 20KHz, 20%
        pio_pwm_set_duty(pio, sm, 20);
        delay_ms(500);
        // 20KHz, 80%
        pio_pwm_set_duty(pio, sm, 80);
        delay_ms(500);
    }
}
