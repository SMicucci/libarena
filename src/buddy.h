#ifndef BUDDYH_H
#define BUDDYH_H

#include "bitmap.h"
#include <pthread.h>
#include <stdint.h>

typedef struct buddy {
        void *mem;
        uint8_t size;
        bitmap_t *map;
        pthread_mutex_t mux;
} buddy_t;

buddy_t *buddy_init(buddy_t *self, bitmap_t *map);

void *buddy_alloc(buddy_t *self, size_t size);

void buddy_free(buddy_t *self, void *mem);

#endif /* end of include guard: BUDDYH_H */
