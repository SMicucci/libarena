#include "bitmap.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

size_t bitmap_init(bitmap_t *self, size_t nelem)
{
        if (!self)
                return 0;
        size_t res = sizeof(bitmap_t) + (nelem + 7) / 8;
        self->nelem = nelem;
        memset(self->data, 0, (nelem + 7) / 8);
        return res;
}

void bitmap_toggle(bitmap_t *self, uint32_t pos)
{
        if (!self || pos >= self->nelem)
                return;
        self->data[pos / 8] ^= 1 << (pos % 8);
}

void bitmap_up(bitmap_t *self, uint32_t pos)
{
        if (!self || pos >= self->nelem)
                return;
        self->data[pos / 8] |= 1 << (pos % 8);
}

void bitmap_down(bitmap_t *self, uint32_t pos)
{
        if (!self || pos >= self->nelem)
                return;
        self->data[pos / 8] &= ~(1 << (pos % 8));
}

int bitmap_get(bitmap_t *self, uint32_t pos)
{
        if (!self || pos >= self->nelem)
                return -1;
        return (self->data[pos / 8] & (1 << (pos % 8))) != 0;
}

void bitmap_range_toggle(bitmap_t *self, uint32_t start, uint32_t end)
{
        if (!self || start >= self->nelem || end >= self->nelem || start > end)
                return;
        uint8_t *data = self->data;
        uint32_t byte_start = start / 8;
        uint32_t byte_end = end / 8;
        uint8_t mask_start = 0xffu << (start % 8);
        uint8_t mask_end = 0xffu >> (7 - (end % 8));

        if (byte_start == byte_end) {
                data[byte_start] ^= (mask_start & mask_end);
                return;
        }
        data[byte_start] ^= mask_start;
        for (uint32_t i = byte_start + 1; i < byte_end; i++)
                data[i] ^= 0xffu;
        data[byte_end] ^= mask_end;
}

void bitmap_range_up(bitmap_t *self, uint32_t start, uint32_t end)
{
        if (!self || start >= self->nelem || end >= self->nelem || start > end)
                return;
        uint8_t *data = self->data;
        uint32_t byte_start = start / 8;
        uint32_t byte_end = end / 8;
        uint8_t mask_start = 0xffu << (start % 8);
        uint8_t mask_end = 0xffu >> (7 - (end % 8));
        if (byte_start == byte_end) {
                data[byte_start] |= (mask_start & mask_end);
                return;
        }
        data[byte_start] |= mask_start;
        for (uint32_t i = byte_start + 1; i < byte_end; i++)
                data[i] = 0xffu;
        data[byte_end] |= mask_end;
}

void bitmap_range_down(bitmap_t *self, uint32_t start, uint32_t end)
{
        if (!self || start >= self->nelem || end >= self->nelem || start > end)
                return;
        uint8_t *data = self->data;
        uint32_t byte_s = start / 8;
        uint32_t byte_e = end / 8;
        uint8_t mask_s = 0xffu << (start % 8);
        uint8_t mask_e = 0xffu >> (7 - (end % 8));
        if (byte_s == byte_e) {
                data[byte_s] &= ~(mask_s & mask_e);
                return;
        }
        data[byte_s] &= ~mask_s;
        for (uint32_t i = byte_s + 1; i < byte_e; i++)
                data[i] = 0x00u;
        data[byte_e] &= mask_e;
}

int64_t bitmap_first_down(bitmap_t *self)
{
        if (!self)
                return -1;
        uint32_t num = self->nelem;
        uint32_t pos = 0;
        uint8_t *p = self->data;
        while (pos + 8 <= num) {
                uint8_t b = *p++;
                if (b != 0xff) {
                        for (uint32_t i = 0; i < 8; i++, pos++) {
                                if (!(b & (1 << i)))
                                        return pos;
                        }
                } else {
                        pos += 8;
                }
        }
        while (pos < num) {
                if (!(*p & (1 << (pos % 8))))
                        return pos;
                pos++;
        }
        return -1;
}

int64_t bitmap_first_up(bitmap_t *self)
{
        if (!self)
                return -1;
        uint32_t num = self->nelem;
        uint32_t pos = 0;
        uint8_t *p = self->data;
        while (pos + 8 <= num) {
                uint8_t b = *p++;
                if (b != 0x00) {
                        for (uint32_t i = 0; i < 8; i++, pos++) {
                                if (b & (1 << i))
                                        return pos;
                        }
                } else {
                        pos += 8;
                }
        }
        while (pos < num) {
                if ((*p & (1 << (pos % 8))))
                        return pos;
                pos++;
        }
        return -1;
}
