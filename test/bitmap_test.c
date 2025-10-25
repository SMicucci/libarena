#include "../src/bitmap.c"
#include "test.h"
#include <stdio.h>

void bitmap_print(bitmap_t *self);

result_t test(void)
{
        result_t res = {0, 0};
        // allocate new bitmap and see if it work
        printf(">\x1b[32m allocate new bitmap (zeroed)\x1b[0m\n");
        res.total++;
        uint8_t mem[16];
        bitmap_t *bitmap = (bitmap_t *)(void *)mem;
        size_t size = bitmap_init(bitmap, 96);
        if (!size)
                return res;
        bitmap_print(bitmap);
        res.passed++;
        // toggle some bits
        printf(">\x1b[32m toggle and play with some bits \x1b[0m\n");
        res.total++;
        for (size_t i = 0; i < bitmap->nelem; i++) {
                if (i < 64)
                        bitmap_up(bitmap, i);
                if ((i % 2) && i < 48 && i > 32)
                        bitmap_down(bitmap, i);
                if ((i % 3) == 2 && i > 48)
                        bitmap_toggle(bitmap, i);
        }
        bitmap_print(bitmap);
        res.passed++;
        // find first 0 and invert all
        printf(">\x1b[32m find first 0 then invert \x1b[0m\n");
        res.total++;
        int64_t pos = bitmap_first_down(bitmap);
        if (pos != 33)
                return res;
        bitmap_range_toggle(bitmap, 0, bitmap->nelem - 1);
        bitmap_print(bitmap);
        res.passed++;
        // find first 0 and invert all
        printf(">\x1b[32m invalid testing \x1b[0m\n");
        res.total++;
        int val = bitmap_get(bitmap, bitmap->nelem);
        if (val != -1) {
                return res;
        }
        bitmap_range_up(bitmap, 0, bitmap->nelem - 1);
        pos = bitmap_first_down(bitmap);
        if (pos != -1) {
                return res;
        }
        bitmap_print(bitmap);
        res.passed++;
        return res;
}

void bitmap_print(bitmap_t *self)
{
        printf("\x1b[34mbitmap\x1b[0m(\x1b[36m%u\x1b[0m):\n", self->nelem);
        size_t i = 0;
        const uint8_t *data = self->data;
        while (i < self->nelem) {
                if (!(i % 16))
                        printf("\t");
                if (data[i / 8] & (1 << (i % 8)))
                        printf("\x1b[32m1\x1b[0m");
                else
                        printf("\x1b[31m0\x1b[0m");
                i++;
                if (!(i % 16))
                        printf("\n");
                else if (!(i % 8))
                        printf(" ");
                else if (!(i % 4))
                        printf("-");
        }
}
