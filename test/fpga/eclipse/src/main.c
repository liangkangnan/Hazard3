/*
 ============================================================================
 Name        : main.c
 Author      : lkn
 Version     :
 Copyright   : Your copyright notice
 Description : Hello RISC-V World in C
 ============================================================================
 */

#include <stdint.h>

#include "uart_drv.h"
#include "printf.h"


int main()
{
    uart_init(115200);

    printf("hello hazard3!!!\n");

    while (1);
}
