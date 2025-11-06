#include "../src/slab.h"
#include "test.h"
#include <stdio.h>

static void slab_print(slab_t *self);

result_t test(void)
{
        result_t res = {0, 0};
        slab_t *slab = slab_new(600, 18);
        if (!slab) {
                printf("invalid input\n");
                return res;
        }
        slab_print(slab);
        int n = slab->map->nelem;
        void *slice[n];
        for (int i = 0; i < n; i++) {
                slice[i] = slab_alloc(slab, 1000);
                // if (!slice[i]) return res;
        }
        slab_print(slab);
        for (int i = 0; i < n; i++) {
                slab_free(slab, slice[i]);
                slice[i] = NULL;
        }
        slab_print(slab);
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
