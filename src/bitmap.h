#ifndef BITMAP_H
#define BITMAP_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Bitmap representing a fixed-size bit array.
 *
 * Unique source of truth. Must be:
 * - Wrapped around a mutex.
 * - Kept unique (no cloning).
 * - Initialized with known memory.
 */
typedef struct bitmap {
        uint32_t nelem;
        uint8_t data[];
} bitmap_t;

#ifndef BITMAP_ERROR
/** @def BITMAP_ERROR
 * error returned by bitmap
 * */
#define BITMAP_ERROR (-1)
#endif /* ifndef BITMAP_ERROR */

/**
 * @brief initialize bitmap from an allocated memory
 *
 * @param self memory to initialize
 *
 * @param nelem number of element to represent
 *
 * @return sizeof variadic bitmap_t struct
 */
size_t bitmap_init(bitmap_t *self, size_t nelem);

/**
 * @brief invert value of a bit
 *
 * @param self bitmap pointer
 *
 * @param pos bit position
 *
 * @warning out of bound access will be silently declined
 */
void bitmap_toggle(bitmap_t *self, uint32_t pos);

/**
 * @brief set up a bit
 *
 * @param self bitmap pointer
 *
 * @param pos bit position
 *
 * @warning out of bound access will be silently declined
 */
void bitmap_up(bitmap_t *self, uint32_t pos);

/**
 * @brief set down a bit
 *
 * @param self bitmap pointer
 *
 * @param pos bit position
 *
 * @warning out of bound access will be silently declined
 */
void bitmap_down(bitmap_t *self, uint32_t pos);

/**
 * @brief retrieve value of a bit
 *
 * @param self bitmap pointer
 *
 * @param pos bit position
 *
 * @return bit value
 *
 * @error out of bound access return -1
 */
int bitmap_get(bitmap_t *self, uint32_t pos);

/**
 * @brief toggle a range of bits
 *
 * @param self bitmap pointer
 *
 * @param start first bit included
 *
 * @param end last bit included
 *
 * @warning out of bound access will be silently declined
 */
void bitmap_range_toggle(bitmap_t *self, uint32_t start, uint32_t end);

/**
 * @brief set up a range of bits
 *
 * @param self bitmap pointer
 *
 * @param start first bit included
 *
 * @param end last bit included
 *
 * @warning out of bound access will be silently declined
 */
void bitmap_range_up(bitmap_t *self, uint32_t start, uint32_t end);

/**
 * @brief set down a range of bits
 *
 * @param self bitmap pointer
 *
 * @param start first bit included
 *
 * @param end last bit included
 *
 * @warning out of bound access will be silently declined
 */
void bitmap_range_down(bitmap_t *self, uint32_t start, uint32_t end);

/**
 * @brief search first down bit
 *
 * @param self bitmap pointer
 *
 * @return poisition of the first bit
 *
 * @error if none is find return -1
 */
int64_t bitmap_first_down(bitmap_t *self);

/**
 * @brief search first up bit
 *
 * @param self bitmap pointer
 *
 * @return poisition of the first bit
 *
 * @error if none is find return -1
 */
int64_t bitmap_first_up(bitmap_t *self);

#endif /* end of include guard: BITMAP_H */
