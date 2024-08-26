#ifndef _MACH_TIMER_DRV_H_
#define _MACH_TIMER_DRV_H_


#define MACH_TIMER_BASE_ADDR    0x40000000

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t RESERVED;
    volatile uint32_t MTIME;
    volatile uint32_t MTIMEH;
    volatile uint32_t MTIMECMP;
    volatile uint32_t MTIMECMPH;
} mach_timer_t;

#define MACH_TIMER ((mach_timer_t *)MACH_TIMER_BASE_ADDR)

void mach_timer_enable(bool en);
void mach_timer_irq_enable(bool en);
void mach_timer_set_cmp_time(uint64_t cmp);
uint64_t mach_timer_get_cmp_time();
void mach_timer_set_time(uint64_t time);
uint64_t mach_timer_get_time();

#endif
