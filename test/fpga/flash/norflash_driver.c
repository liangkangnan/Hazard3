#include "flash_drv.h"
#include "norflash_driver.h"


void norflash_init()
{
    flash_set_cs(1);
}

static void send_cmd(uint8_t cmd)
{
    flash_set_cs(0);

    flash_write(&cmd, 1);

    flash_set_cs(1);
}

static uint8_t read_reg(uint8_t cmd)
{
    uint8_t data;

    flash_set_cs(0);

    flash_write(&cmd, 1);
    flash_read(&data, 1);

    flash_set_cs(1);

    return data;
}

static uint8_t is_busy()
{
    if (read_reg(FLASH_CMD_READ_STATUS) & 0x1)
        return 1;
    else
        return 0;
}

void norflash_write_enable()
{
    send_cmd(FLASH_CMD_WRITE_ENABLE);
}

void norflash_write_disable()
{
    send_cmd(FLASH_CMD_WRITE_DISABLE);
}

void norflash_sector_erase(uint32_t sector_addr)
{
    uint8_t buf[4];

    buf[0] = FLASH_CMD_SECTOR_ERASE;
    buf[1] = (sector_addr >> 16) & 0xff;
    buf[2] = (sector_addr >> 8) & 0xff;
    buf[3] = (sector_addr >> 0) & 0xff;

    norflash_write_enable();

    flash_set_cs(0);
    flash_write(buf, 4);
    flash_set_cs(1);

    while (is_busy());
}

void norflash_read(uint32_t addr, uint8_t *buffer, uint32_t len)
{
    uint8_t buf[4];

    buf[0] = FLASH_CMD_READ;
    buf[1] = (addr >> 16) & 0xff;
    buf[2] = (addr >> 8) & 0xff;
    buf[3] = (addr >> 0) & 0xff;

    flash_set_cs(0);

    flash_write(buf, 4);
    flash_read(buffer, len);

    flash_set_cs(1);
}

void norflash_page_write(uint32_t addr, uint8_t *buffer, uint32_t len)
{
    uint8_t buf[4];

    buf[0] = FLASH_CMD_PAGE_PROG;
    buf[1] = (addr >> 16) & 0xff;
    buf[2] = (addr >> 8) & 0xff;
    buf[3] = (addr >> 0) & 0xff;

    norflash_write_enable();

    flash_set_cs(0);
    flash_write(buf, 4);
    flash_write(buffer, len);
    flash_set_cs(1);

    while (is_busy());
}
