#ifndef _FLASH_DRV_H_
#define _FLASH_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#include "spi_03h_xip_regs.h"

#define FLASH_BASE_ADDR  (0x40008000)

typedef struct {
    volatile uint32_t CSR;
    volatile uint32_t TX;
    volatile uint32_t RX;
} flash_t;

#define FLASH ((flash_t *)FLASH_BASE_ADDR)

void flash_enter_cmd_mode();
void flash_exit_cmd_mode();
uint8_t flash_busy();
void flash_set_cs(uint8_t level);
void flash_write(uint8_t *buf, uint32_t len);
void flash_read(uint8_t *buf, uint32_t len);

#endif
