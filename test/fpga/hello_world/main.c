#include <stdint.h>

#include "uart_drv.h"
#include "xprintf.h"


int main()
{
    uart_init(115200);
    xdev_out(uart_putc);

    //uart_puts("hello world!\n");
    xprintf("hellow!!!\n");

    while (1);
}
