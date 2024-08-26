#include <stdint.h>
#include "uart_drv.h"

void __attribute__((interrupt)) handle_exception()
{
    uart_init(115200);
    uart_puts("handle_exception!!!\n");

    while (1);
}
