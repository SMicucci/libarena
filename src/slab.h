#ifndef SLAB_H
#define SLAB_H

#include "bitmap.h"
#include <pthread.h>
#include <stdint.h>

typedef struct slab {
        void *mem;
        uint32_t size;
        bitmap_t *map;
        pthread_mutex_t mux;
} slab_t;

slab_t *slab_new(uint32_t size, uint32_t nelem);
void *slab_alloc(slab_t *self, size_t size);
void slab_free(slab_t *self, void *ptr);

#endif /* end of include guard: SLAB_H */
