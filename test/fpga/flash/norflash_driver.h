#ifndef _NORFLASH_DRIVER_H_
#define _NORFLASH_DRIVER_H_

#define FLASH_CMD_READ          0x03
#define FLASH_CMD_PAGE_PROG     0x02
#define FLASH_CMD_WRITE_ENABLE  0x06
#define FLASH_CMD_WRITE_DISABLE 0x04
#define FLASH_CMD_SECTOR_ERASE  0x20
#define FLASH_CMD_READ_STATUS   0x05

void norflash_init();
void norflash_sector_erase(uint32_t sector_addr);
void norflash_read(uint32_t addr, uint8_t *buffer, uint32_t len);
void norflash_page_write(uint32_t addr, uint8_t *buffer, uint32_t len);

#endif
