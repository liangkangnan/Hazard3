#include "flash_drv.h"

void flash_enter_cmd_mode()
{
    FLASH->CSR |= 1 << XIP_CSR_DIRECT_LSB;
}

void flash_exit_cmd_mode()
{
    FLASH->CSR &= ~(1 << XIP_CSR_DIRECT_LSB);
}

uint8_t flash_busy()
{
    if (FLASH->CSR & (1 << XIP_CSR_BUSY_LSB))
        return 1;
    else
        return 0;
}

void flash_set_cs(uint8_t level)
{
    if (level)
        flash_exit_cmd_mode();
    else
        flash_enter_cmd_mode();
}

void flash_write(uint8_t *buf, uint32_t len)
{
    uint32_t i;

    while (flash_busy());

    for (i = 0; i < len; i++) {
        FLASH->TX = buf[i];
        while (flash_busy());
    }
}

void flash_read(uint8_t *buf, uint32_t len)
{
    uint32_t i;

    while (flash_busy());

    for (i = 0; i < len; i++) {
        FLASH->TX = 0xff;
        while (flash_busy());
        buf[i] = FLASH->RX;
    }
}
