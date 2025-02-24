#include <stdint.h>

#include "uart_drv.h"
#include "printf.h"
#include "pio_drv.h"
#include "perireset_drv.h"

PIO pio = pio0;


int main()
{
    uart_init(115200);

    printf("hello peri_reset!!!\n");

    printf("before reset:\n");
    printf("0x%x\n", pio->sm[0].clkdiv);
    printf("0x%x\n", pio->sm[0].pinctrl);

    printf("after write:\n");
    pio->sm[0].clkdiv = 0x12345678;
    pio->sm[0].pinctrl = 0x12345678;
    printf("0x%x\n", pio->sm[0].clkdiv);
    printf("0x%x\n", pio->sm[0].pinctrl);

    printf("after reset:\n");
    peri_reset(PIO0_RESET_BIT);
    printf("0x%x\n", pio->sm[0].clkdiv);
    printf("0x%x\n", pio->sm[0].pinctrl);

    while (1);
}
