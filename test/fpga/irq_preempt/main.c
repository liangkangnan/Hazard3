#include <stdint.h>
#include <stdbool.h>

#include "uart_drv.h"
#include "printf.h"
#include "hazard3_irq.h"


uint8_t external_irq_entry_count = 0;

void handler();

int main()
{
    uart_init(115200);

    printf("hello hazard3!!!\n");

    global_irq_enable(true);
    external_irq_enable(true);
    // Enable one external IRQ at each priority level
    for (int i = 0; i <= MAX_PRIORITY; ++i) {
        h3irq_enable(i, true);
        h3irq_set_priority(i, i);
        h3irq_set_external_irq_handler(i, handler);
    }
    // Set off the lowest-priority IRQ. The IRQ handler will then set the
    // next-lowest, which will preempt it. So on, up to the highest level,
    // then return all the way back down through the nested frames.
    printf("Posting first IRQ\n");
    h3irq_force_pending(0, true);

    printf("EIRQ vector was entered %d times\n", external_irq_entry_count);

    while (1);
}

void handler()
{
    external_irq_entry_count++;

    int irqnum = h3irq_get_current_irq();
    printf("Entered IRQ %d\n", irqnum);
    if (irqnum < MAX_PRIORITY)
        h3irq_force_pending(irqnum + 1, true);
    // !!! Get preempted here
    h3irq_force_pending(irqnum, false);
    // Make sure context save/restore tracks as expected:
    irqnum = h3irq_get_current_irq();
    printf("Exiting IRQ %d\n", irqnum);
}
