#ifndef BASE_H
#define BASE_H

#include <stddef.h>

/**
 * allocate huge pages
 * side effect on the size if not aligned with page sizes
 * errno are forwarder in case of error
 *
 * use `mmap()` under the table
 */
void *page_alloc(size_t *size);

/**
 * free correctly pages
 * errno are forwarder in case of error
 *
 * use `mmap()` under the table
 */
void page_free(void *ptr, size_t size);

#endif /* end of include guard: BASE_H */
