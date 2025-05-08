#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

void queue_init(queue_t *q, uint8_t *buf, uint16_t element_size, uint16_t element_count)
{
    q->data = buf;
    q->element_count = element_count;
    q->element_size = element_size;
    q->wptr = 0;
    q->rptr = 0;
    q->level = 0;
}

static inline void *element_ptr(queue_t *q, uint32_t index)
{
    return (q->data + index * q->element_size);
}

static inline uint16_t inc_index(queue_t *q, uint16_t index)
{
    if (++index >= q->element_count) {
        index = 0;
    }

    return index;
}

static uint8_t queue_add_internal(queue_t *q, void *data, uint8_t block)
{
    QUEUE_LOCK();

    do {
        if (queue_get_level(q) != q->element_count) {
            memcpy(element_ptr(q, q->wptr), data, q->element_size);
            q->wptr = inc_index(q, q->wptr);
            q->level++;
            QUEUE_UNLOCK();
            return 1;
        }
        if (!block)
            break;
    } while (1);

    QUEUE_UNLOCK();

    return 0;
}

static uint8_t queue_remove_internal(queue_t *q, void *data, uint8_t block)
{
    QUEUE_LOCK();

    do {
        if (queue_get_level(q) != 0) {
            memcpy(data, element_ptr(q, q->rptr), q->element_size);
            q->rptr = inc_index(q, q->rptr);
            q->level--;
            QUEUE_UNLOCK();
            return 1;
        }
        if (!block)
            break;
    } while (1);

    QUEUE_UNLOCK();

    return 0;
}

static uint8_t queue_peek_internal(queue_t *q, void *data, uint8_t block)
{
    QUEUE_LOCK();

    do {
        if (queue_get_level(q) != 0) {
            memcpy(data, element_ptr(q, q->rptr), q->element_size);
            QUEUE_UNLOCK();
            return 1;
        }
        if (!block)
            break;
    } while (1);

    QUEUE_UNLOCK();

    return 0;
}

uint8_t queue_try_add(queue_t *q, void *data)
{
    return queue_add_internal(q, data, 0);
}

uint8_t queue_try_remove(queue_t *q, void *data)
{
    return queue_remove_internal(q, data, 0);
}

uint8_t queue_try_peek(queue_t *q, void *data)
{
    return queue_peek_internal(q, data, 0);
}

void queue_add_blocking(queue_t *q, void *data)
{
    queue_add_internal(q, data, 1);
}

void queue_remove_blocking(queue_t *q, void *data)
{
    queue_remove_internal(q, data, 1);
}

void queue_peek_blocking(queue_t *q, void *data)
{
    queue_peek_internal(q, data, 1);
}
