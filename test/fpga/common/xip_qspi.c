#include "flash_drv.h"
#include "sections.h"

static void __not_in_flash_func(__flash_write_enable)()
{
    uint8_t cmd = 0x06;

    flash_set_cs(0);

    flash_write(&cmd, 1);

    flash_set_cs(1);
}

static void __not_in_flash_func(__flash_qspi_mode_enable)()
{
    uint8_t cmd = 0x01;
    uint8_t data[2] = {0x00, 0x02};

    flash_set_cs(0);

    flash_write(&cmd, 1);
    flash_write(data, 2);

    flash_set_cs(1);
}

void __not_in_flash_func(xip_qspi_init)()
{
    flash_set_cs(1);
    flash_enter_cmd_mode();

    __flash_write_enable();
    __flash_qspi_mode_enable();
    flash_set_clock_divider(1);
    flash_set_qspi_dummy_count(4);
    flash_set_qspi_mode_enabled(1);
    flash_exit_cmd_mode();

    flash_set_icache_enabled(1);
}
