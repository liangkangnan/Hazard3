#include <stdint.h>

#include "uart_drv.h"
#include "printf.h"


int main()
{
    uart_init(115200);

    printf("hello hazard3!!!\n");

    while (1);
}
