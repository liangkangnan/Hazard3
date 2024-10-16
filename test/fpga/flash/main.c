#include <stdint.h>

#include "norflash_driver.h"
#include "uart_drv.h"
#include "printf.h"
#include "sim_ctrl.h"

static uint8_t write_buffer[256];
static uint8_t read_buffer[256];

#define LEN 256

int main()
{
	uart_init(115200);
    printf("hello flash\n");

    norflash_init();
    norflash_sector_erase(0x00);

    for (uint32_t i = 0; i < LEN; i++)
        write_buffer[i] = i;
    norflash_page_write(0x00, write_buffer, LEN);

    norflash_read(0x00, read_buffer, LEN);
    uint8_t succ = 1;
    for (uint32_t i = 0; i < LEN; i++) {
        //printf("read buf[%d] = 0x%x\n", i, read_buffer[i]);
        if (read_buffer[i] != write_buffer[i]) {
            printf("error at %d\n", i);
            succ = 0;
            break;
        }
    }

    if (succ)
        printf("succ\n");
    else
        printf("fail\n");

    printf("end.\n");

    while (1) {

    }
}
