#include "../src/slab.h"
#include "test.h"
#include <stdio.h>

static void slab_print(slab_t *self);

result_t test(void)
{
        result_t res = {4, 0};
        const size_t size = 600;
        printf(">\x1b[32m try allocation of a slab\x1b[0m\n");
        slab_t *slab = slab_new(size, 18);
        if (!slab) {
                printf("invalid input\n");
                return res;
        }
        slab_print(slab);
        size_t frag =
            (sizeof(slab_t) + sizeof(bitmap_t) + slab->size * slab->map->nelem +
             (slab->map->nelem + 7) / 8);
        frag = (frag & 0xfffffffffffff000) - frag;
        printf("# internal fragmentation: %lu\n", frag);
        res.passed++;
        printf(">\x1b[32m request allocation till completion\x1b[0m\n");
        int n = slab->map->nelem;
        void *slice[n];
        for (int i = 0; i < n; i++) {
                slice[i] = slab_alloc(slab, size);
                if (!slice[i])
                        return res;
        }
        slab_print(slab);
        res.passed++;
        printf(">\x1b[32m fail allocation when out of slab and excessive "
               "size\x1b[0m\n");
        void *failed = NULL;
        failed = slab_alloc(slab, size);
        if (failed) {
                return res;
        }
        failed = slab_alloc(slab, size + 1);
        if (failed) {
                return res;
        }
        slab_print(slab);
        res.passed++;
        printf(">\x1b[32m free all allocated block\x1b[0m\n");
        for (int i = 0; i < n; i++) {
                slab_free(slab, slice[i]);
                slice[i] = NULL;
        }
        slab_print(slab);
        res.passed++;
        return res;
}

static void slab_print(slab_t *self)
{
        if (!self) {
                printf("nullptr excp\n");
                return;
        }
        printf("\x1b[34mslab\x1b[0m<\x1b[35m%u\x1b[0m>(\x1b[36m%u\x1b[0m): "
               "[\x1b[33m%u\x1b[0m]\n",
               self->size, self->map->nelem, self->size * self->map->nelem);
        size_t i = 0;
        const uint8_t *data = self->map->data;
        while (i < self->map->nelem) {
                if (!(i % 32))
                        printf("\t");
                if (data[i / 8] & (1 << (i % 8)))
                        printf("\x1b[32m1\x1b[0m");
                else
                        printf("\x1b[31m0\x1b[0m");
                i++;
                if (!(i % 32))
                        printf("\n");
                else if (!(i % 8))
                        printf(" ");
                else if (!(i % 4))
                        printf("-");
        }
        if ((self->map->nelem % 32))
                printf("\n");
}
