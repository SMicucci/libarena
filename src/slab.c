#include "slab.h"
#include "base.h"
#include "bitmap.h"
#include <bits/pthreadtypes.h>
#include <errno.h>
#include <pthread.h>
#include <sanitizer/asan_interface.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#ifndef PAD
#define PAD 8
#endif /* ifndef PAD */

// helper to init slab
// split base logic from implementaion
static inline void slab_init(slab_t *self, void *mem, bitmap_t *map,
                             size_t size);

slab_t *slab_new(uint32_t size, uint32_t nelem)
{
        // minimum size + asan sanitize
        if (size < 32)
                return NULL;
        size += (2 * PAD);
        // max capacity declared
        if ((size_t)size * (size_t)nelem > UINT32_MAX)
                return NULL;
        size_t req;
        //
        // when undefined nelem use a huge page of 2MB
        // if declared calculate minimum request
        //
        if (!nelem) {
                req = (1 << 21);
        } else {
                // slab_t + mem + bitmap [ceil(mem/size/8) + 8]
                req = sizeof(slab_t) + (size_t)nelem * size;
                req += bitmap_sizeof(size, req);
        }
        nelem = (req - sizeof(slab_t) -
                 bitmap_sizeof(size, req - sizeof(slab_t)) + size - 1) /
                size;
        //
        // allocate page
        //
        void *mem = page_alloc(&req);
        if (!mem) {
                // on error comunicate it
                fprintf(stdout, "mmap failed: %s\n", strerror(errno));
                return NULL;
        }
        //
        // arithmetic ptr utility
        //
        uint8_t *ptr = (uint8_t *)mem;
        // sizeof bitmap
        slab_t *self = (slab_t *)ptr; /* mem[0]: slab ptr */
        bitmap_t *map = (bitmap_t *)(ptr + sizeof(*self)); /* mem[64]: bitmap */
        size_t offset = bitmap_init(map, nelem);      /* offset from bitmap */
        mem = (void *)(ptr + offset + sizeof(*self)); /* mem [64+off]: arena */
        //
        // init slab with parsed part of memory
        //
        slab_init(self, mem, map, size);
        return self;
}

void *slab_alloc(slab_t *self, size_t size)
{
        if (!self)
                return NULL;
        if (size > PAD + self->size + PAD)
                return NULL;
        int64_t pos = bitmap_first_down(self->map);
        if (pos == -1)
                return NULL;
        bitmap_up(self->map, pos);
        uint8_t *ptr = self->mem;
        void *res = ptr + (self->size * pos) + PAD;
        __asan_unpoison_memory_region(res, size);
        return res;
}

void slab_free(slab_t *self, void *ptr)
{
        if (!self)
                return;
        if (!ptr)
                return;
        ptr = (void *)((uint8_t *)ptr - PAD);
        if ((uint8_t *)ptr < (uint8_t *)self->mem ||
            (uint8_t *)ptr >=
                ((uint8_t *)self->mem) +
                    (uint64_t)self->size * (uint64_t)self->map->nelem)
                return;
        int64_t pos = ((uint8_t *)ptr - (uint8_t *)self->mem) / self->size;
        bitmap_down(self->map, pos);
        __asan_poison_memory_region(ptr, self->size);
}

static inline void slab_init(slab_t *self, void *mem, bitmap_t *map,
                             size_t size)
{
        if (!self || !mem || !map)
                return;
        self->map = map;
        self->mem = mem;
        self->size = size;
        pthread_mutex_init(&self->mux, NULL);
        // poison all memory at first
        __asan_poison_memory_region(self->mem, self->map->nelem * self->size);
}
