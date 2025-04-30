// 两路互补带死区PWM输出
//  ______           ____________           ____________           ____________
//        |         |            |         |            |         |
//        |_________|            |_________|            |_________|
//
//           _____                  _____                  _____
//          |     |                |     |                |     |
//  ________|     |________________|     |________________|     |______________
//
//        | |     | |
//        | |     | |
//     dead_time  dead_time
//

#include <stdint.h>
#include <stdbool.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "pio_instructions.h"
#include "printf.h"
#include "pwm.h"
#include "delay.h"


#define PWM_IO  8

PIO pio;
uint32_t sm;

// 占空比：0%~128%，step=1%
void pio_pwm_set_duty(PIO pio, uint32_t sm, uint32_t duty, uint32_t dead_time)
{
    uint32_t low_count;
    uint32_t data[PIO_SM_FIFO_COUNT];
    uint32_t *p;
    uint32_t i, j;

    while (pio_sm_get_tx_fifo_shadow_update_state(pio, sm));

    if (duty > 128)
        duty = 128;

    low_count = 128 - duty;

    for (i = 0; i < PIO_SM_FIFO_COUNT; i++)
        data[i] = 0x00;

    p = data;
    for (i = 0, j = 0; i < 128; i++, j++) {
        if (j >= 16) {
            p++;
            j = 0;
        }
        if (i < low_count) {
            if ((i >= dead_time) && (i < (low_count - dead_time)))
                *p |= 0x2 << (j * 2);
        } else {
            *p |= 0x1 << (j * 2);
        }
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

int8_t pwm_init()
{
    int offset = pio_add_program(&pio, &sm, &pwm_program);
    if (offset < 0) {
        printf("Not enough space for sm!\n");
        return -1;
    }

    printf("pio=%d, sm=%d\n", pio_get_index(pio), sm);

    pio_sm_config config = {0};
    pio_sm_config_set_out_pins(&config, PWM_IO, 2);
    pio_sm_config_set_out_shift(&config, true, true, 32);
    pio_sm_config_set_wrap(&config, offset + pwm_wrap_bottom, offset + pwm_wrap_top);
    pio_sm_config_set_clkdiv(&config, 6, 0);
    pio_sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_TX);

    pio_sm_set_consecutive_pindirs(pio, sm, PWM_IO, 2, true);
    pio_sm_init(pio, sm, offset, &config);

    pio_sm_set_tx_fifo_shadow_mode_enabled(pio, sm, 1);

    // init duty: 0%
    pio_sm_clear_fifos(pio, sm);
    for (uint8_t i = 0; i < PIO_SM_FIFO_COUNT; i++)
        pio_sm_put(pio, sm, 0x00);
    pio_sm_clear_fifos(pio, sm);

    return 0;
}

int main()
{
    uart_init(115200);

    printf("hello pio pwm!!!\n");

    pwm_init();

    pio_sm_set_enabled(pio, sm, true);

    printf("pio pwm started\n");

    //pio_pwm_set_duty(pio, sm, 50, 2);

    while (1) {
        // 15.6KHz, 10%
        pio_pwm_set_duty(pio, sm, 10, 2);
        delay_ms(500);
        // 15.6KHz, 90%
        pio_pwm_set_duty(pio, sm, 90, 2);
        delay_ms(500);
    }
}
