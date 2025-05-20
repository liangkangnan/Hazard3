#include <stdint.h>

#include "uart_drv.h"
#include "printf.h"
#include "crc_drv.h"
#include "delay.h"

#include "data.h"

// software crc32
uint32_t software_crc32(uint8_t *data, uint32_t length)
{
    uint8_t i;
    uint32_t crc = 0xffffffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;                // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;// 0xEDB88320 = reverse 0x04C11DB7
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;
}

// hardware crc32
uint32_t hardware_crc32(uint8_t *data, uint32_t length)
{
    crc_enable(0);
    crc_set_init_val(0xffffffff);
    // 0xEDB88320 = reverse 0x04C11DB7
    crc_set_poly_val(0xEDB88320);
    crc_enable(1);

    while (length--) {
        crc_put_data_blocking(*data++);
    }

    while (!crc_is_idle());

    return (~crc_get_result());
}


int main()
{
    uint32_t crc;
    uint64_t start;
    uint64_t end;

	uart_init(115200);

    printf("hello crc\n");

    start = riscv_get_cycle_time();
    crc = software_crc32((uint8_t *)cal_data, sizeof(cal_data));
    end = riscv_get_cycle_time();
    printf("s crc = 0x%x\n", crc);
    printf("s cycle = %lld\n", (uint64_t)(end - start));

    start = riscv_get_cycle_time();
    crc = hardware_crc32((uint8_t *)cal_data, sizeof(cal_data));
    end = riscv_get_cycle_time();
    printf("h crc = 0x%x\n", crc);
    printf("h cycle = %lld\n", (uint64_t)(end - start));

    while (1) {

    }
}
