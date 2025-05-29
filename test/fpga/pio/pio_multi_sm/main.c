#include <stdint.h>

#include "uart_drv.h"
#include "pio_drv.h"
#include "printf.h"
#include "multi_sm.h"

#define LED0_PIN 8
#define LED1_PIN 9
#define LED2_PIN 10
#define LED3_PIN 11

#define SM_NUM  4

PIO pio[SM_NUM];
uint32_t sm[SM_NUM];
int offset[SM_NUM];

int main()
{
    uart_init(115200);

    printf("hello pio multi sm!!!\n");

    uint32_t len_pin[SM_NUM] = {LED0_PIN, LED1_PIN, LED2_PIN, LED3_PIN};

    for (int i = 0; i < SM_NUM; i++) {
        offset[i] = pio_add_program(&pio[i], &sm[i], &multi_sm_program);
        if (offset[i] < 0) {
            printf("Not enough space for sm%d!\n", i);
            return -1;
        }
        printf("pio=%d, sm=%d, offset=%d\n", pio_get_index(pio[i]), sm[i], offset[i]);

        pio_sm_config config = {0};
        pio_sm_config_set_set_pins(&config, len_pin[i], 1);
        pio_sm_config_set_wrap(&config, offset[i] + multi_sm_wrap_bottom, offset[i] + multi_sm_wrap_top);
        pio_sm_config_set_instr_offset(&config, offset[i]);
        pio_sm_config_set_clkdiv(&config, 12000000 / (i + 1), 0);
        pio_sm_set_consecutive_pindirs(pio[i], sm[i], len_pin[i], 1, true);
        pio_sm_init(pio[i], sm[i], 0, &config);
        pio_sm_set_enabled(pio[i], sm[i], true);
    }

    printf("pio multi sm started\n");

    while (1);
}
