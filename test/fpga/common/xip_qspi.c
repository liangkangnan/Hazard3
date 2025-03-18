#include "flash_drv.h"
#include "sections.h"

static void __not_in_flash_func(__flash_read_status_reg)(uint8_t reg, uint8_t *data)
{
    uint8_t cmd;

    if (reg == 1)
        cmd = 0x05;
    else
        cmd = 0x35;

    flash_set_cs(0);

    flash_write(&cmd, 1);
    flash_read(data, 1);

    flash_set_cs(1);
}

static void __not_in_flash_func(__flash_write_enable)()
{
    uint8_t cmd = 0x06;
    uint8_t data;

    flash_set_cs(0);

    flash_write(&cmd, 1);

    flash_set_cs(1);

    do {
        __flash_read_status_reg(0x1, &data);
    } while (!(data & 0x2));
}

static void __not_in_flash_func(__flash_qspi_mode_enable)()
{
    uint8_t cmd = 0x31;
    uint8_t data;

    __flash_read_status_reg(0x2, &data);

    // 若QE位未置1，则设置
    if ((data & 0x2) == 0) {
        __flash_write_enable();

        data = 0x02;

        flash_set_cs(0);

        flash_write(&cmd, 1);
        flash_write(&data, 1);

        flash_set_cs(1);

        do {
            __flash_read_status_reg(0x2, &data);
        } while (!(data & 0x2));
    }
}

void __not_in_flash_func(xip_qspi_init)()
{
    flash_set_cs(1);
    flash_enter_cmd_mode();

    __flash_qspi_mode_enable();
    flash_set_clock_divider(1);
    flash_set_qspi_dummy_count(4);
    flash_set_qspi_mode_enabled(1);
    flash_exit_cmd_mode();

    flash_set_icache_enabled(1);
}
