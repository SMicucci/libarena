#ifndef SLAB_H
#define SLAB_H

#include "bitmap.h"
#include <pthread.h>
#include <stdint.h>

/** @brief slab allocator MT-safe
 *
 * */
typedef struct slab {
        void *mem;
        uint32_t size;
        bitmap_t *map;
        pthread_mutex_t mux;
} slab_t;

/** @brief create a new slab container optimizing size
 *
 * @param size memory block (slab) dimension
 * @param nelem minimum number of slab
 * */
slab_t *slab_new(uint32_t size, uint32_t nelem);

/** @brief allocate a memory block
 *
 * @param self slab instance
 * @param size memory size
 *
 * @return memory address sanitized
 *
 * @error
 * */
void *slab_alloc(slab_t *self, size_t size);

/** @brief deallocate memory owned by slab, on error ignore silently
 * */
void slab_free(slab_t *self, void *ptr);

#endif /* end of include guard: SLAB_H */
