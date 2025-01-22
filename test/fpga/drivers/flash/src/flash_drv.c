#include "flash_drv.h"
#include "sections.h"

void __not_in_flash_func(flash_set_clock_divider)(uint8_t div)
{
    FLASH->DIV = div;
}

void __not_in_flash_func(flash_enter_cmd_mode)()
{
    FLASH->CSR |= 1 << XIP_CSR_DIRECT_LSB;
}

void __not_in_flash_func(flash_exit_cmd_mode)()
{
    FLASH->CSR &= ~(1 << XIP_CSR_DIRECT_LSB);
}

uint8_t __not_in_flash_func(flash_busy)()
{
    if (FLASH->CSR & (1 << XIP_CSR_BUSY_LSB))
        return 1;
    else
        return 0;
}

void __not_in_flash_func(flash_set_cs)(uint8_t level)
{
    if (level)
        FLASH->CSR |= 1 << XIP_CSR_CS_LEVEL_LSB;
    else
        FLASH->CSR &= ~(1 << XIP_CSR_CS_LEVEL_LSB);
}

void __not_in_flash_func(flash_write)(uint8_t *buf, uint32_t len)
{
    uint32_t i;

    while (flash_busy());

    for (i = 0; i < len; i++) {
        FLASH->TX = buf[i];
        while (flash_busy());
    }

    while (flash_busy());
}

void __not_in_flash_func(flash_read)(uint8_t *buf, uint32_t len)
{
    uint32_t i;

    while (flash_busy());

    for (i = 0; i < len; i++) {
        FLASH->TX = 0xff;
        while (flash_busy());
        buf[i] = FLASH->RX;
    }

    while (flash_busy());
}

void __not_in_flash_func(flash_set_qspi_mode_enabled)(uint8_t en)
{
    if (en)
        FLASH->QSPI_CTRL |= 1 << XIP_QSPI_CTRL_MODE_LSB;
    else
        FLASH->QSPI_CTRL &= ~(1 << XIP_QSPI_CTRL_MODE_LSB);
}

void __not_in_flash_func(flash_set_qspi_dummy_count)(uint8_t count)
{
    FLASH->QSPI_CTRL &= ~XIP_QSPI_CTRL_DUMMY_MASK;
    FLASH->QSPI_CTRL |= count << XIP_QSPI_CTRL_DUMMY_LSB;
}

void __not_in_flash_func(flash_set_icache_enabled)(uint8_t en)
{
    if (en)
        FLASH->ICACHE_CTRL |= 1 << XIP_ICACHE_CTRL_EN_LSB;
    else
        FLASH->ICACHE_CTRL &= ~(1 << XIP_ICACHE_CTRL_EN_LSB);
}

void __not_in_flash_func(flash_flush_icache_blocking)()
{
    FLASH->ICACHE_CTRL |= 1 << XIP_ICACHE_CTRL_FLUSH_LSB;

    while (FLASH->ICACHE_CTRL & (1 << XIP_ICACHE_CTRL_FLUSH_LSB));
}
