#ifndef SIMPLEFS_UTILS_H
#define SIMPLEFS_UTILS_H

#include <stdint.h>
#include <string.h>

#define member_size(type, member) sizeof(((type *)0)->member)


uint32_t inner_fs_get_position_in_8bit(uint8_t bits);

uint32_t inner_fs_get_position_in_16bit(uint16_t bits);

uint32_t inner_fs_get_position_in_32bit(uint32_t bits);

/**
 * @brief Find index of some free struct.
 * 
 * @param block_ptr - pointer to the bitmap block.
 * @param maxOffset - maximum number of some structures in block. All of those indexed structures must be usable.
 * @return uint32_t - UINT32_MAX if some error happend or
 * 0 if there is no more free indexes.
 * Otherwise index of an free index.
 */
uint32_t inner_fs_find_free_index(void* bitmap_ptr, uint32_t maxOffset);

/**
 * @brief Mark index in bitmap as used.
 * 
 * @param bitmap_ptr - pointer to some bitmap. It must be an absolute address.
 * @param bitmapIndex - position in bitmap that will be marked as used.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t inner_fs_mark_bitmap_bit(void* bitmap_ptr, uint32_t bitmapIndex);

/**
 * @brief Mark index in bitmap as free (not used).
 * 
 * @param bitmap_ptr - pointer to some bitmap. It must be an absolute address.
 * @param bitmapIndex - position in bitmap that will be marked as used.
 * @return int8_t - 0 if operation was successful.
 * No other errors.
 */
int8_t inner_fs_free_bitmap_bit(void* bitmap_ptr, uint32_t bitmapIndex);

#endif