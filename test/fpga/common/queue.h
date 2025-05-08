#ifndef _QUEUE_H_
#define _QUEUE_H_

//#include "hazard3_irq.h"

//#define QUEUE_LOCK()      global_irq_enable(0)
//#define QUEUE_UNLOCK()    global_irq_enable(1)
#define QUEUE_LOCK()      
#define QUEUE_UNLOCK()    

typedef struct {
    uint8_t *data;
    uint16_t wptr;
    uint16_t rptr;
    uint16_t element_size;
    uint16_t element_count;
    uint16_t level;
} queue_t;

static inline uint32_t queue_get_level(queue_t *q)
{
    return (q->level);
}

static inline uint8_t queue_is_empty(queue_t *q)
{
    return queue_get_level(q) == 0;
}

static inline uint8_t queue_is_full(queue_t *q)
{
    return queue_get_level(q) == q->element_count;
}

void queue_init(queue_t *q, uint8_t *buf, uint16_t element_size, uint16_t element_count);
uint8_t queue_try_add(queue_t *q, void *data);
uint8_t queue_try_remove(queue_t *q, void *data);
uint8_t queue_try_peek(queue_t *q, void *data);
void queue_add_blocking(queue_t *q, void *data);
void queue_remove_blocking(queue_t *q, void *data);
void queue_peek_blocking(queue_t *q, void *data);

#endif
