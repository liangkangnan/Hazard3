#include <stdint.h>

#include "uart_drv.h"



int main()
{
    uart_init(115200);

    uart_puts("hello world!\n");

    while (1);
}
