#include "base.h"
#include <errno.h>
#include <linux/mman.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

void *page_alloc(size_t *size)
{
        if (!size)
                return NULL;
        if (*size < (1 << 12))
                *size = (1 << 12);
        size_t p_size = *size >= (1 << 30) ? (1 << 30) : *size >= (1 << 21) ? (1 << 21) : (1 << 12);
        // round up size to align
        *size = (*size + p_size - 1) & ~(p_size - 1);
        int prot = PROT_READ | PROT_WRITE;
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;
        if (p_size != (1 << 12))
                flags |= MAP_HUGETLB | (p_size == (1 << 30) ? MAP_HUGE_1GB : MAP_HUGE_2MB);
        void *ptr = mmap(NULL, *size, prot, flags, -1, 0);
        if (ptr == MAP_FAILED && p_size != (1 << 12)) {
                flags &= ~(MAP_HUGETLB | MAP_HUGE_1GB | MAP_HUGE_2MB);
                ptr = mmap(NULL, *size, prot, flags, -1, 0);
                fprintf(stderr, "> mmap [huge] failed: %s\n", strerror(errno));
        }
        if (ptr == MAP_FAILED) {
                fprintf(stderr, "> mmap failed: %s\n", strerror(errno));
                return NULL;
        }
        return ptr;
}

void page_free(void *ptr, size_t size)
{
        munmap(ptr, size);
        fprintf(stderr, "mmap failed: %s\n", strerror(errno));
}
