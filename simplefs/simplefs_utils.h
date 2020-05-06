#ifndef SIMPLEFS_UTILS_H
#define SIMPLEFS_UTILS_H

#include "simplefs_internals.h"
#include <stdint.h>


// Initialize filesystem with set parameters.
void simplefs_init();

// Unlink shared memory segment used by filesystem.
void simplefs_unlink();

// Return count of used resources.
void used_inodes_count();
void used_blocks_count();
void used_rows_count();

// Print all useful information about filesystem.
void print_simplefs_stats();

// Find in inode_bitmap first unset bit
// and return its position.
void find_free_inode();

// Find in block_bitmap first unset bit
// and return its position.
void find_free_block();

// Find in open_file_bitmap first unset bit
// and return its position. 
void find_free_row();

// Generic function to find first unset bit
// in provided array.
int find_free_bit(uint8_t *bitmap, int size);

// Set bit in inode_bitmap and
// increment by 1 used inodes count.
void use_inode();

// Set bit in block_bitmap and
// increment by 1 used blocks count.
void use_block();

// Set bit in open_file_bitmap and
// increment by 1 used rows in open file table count.
void use_row();

// Generic function to set bit in provided array.
void set_bit();

// Unset bit in inode_bitmap and
// decrement by 1 used inodes count.
void free_inode();

// Unset bit in inode_bitmap and
// decrement by 1 used blocks count.
void free_block();

// Unset bit in inode_bitmap and
// decrement by 1 used rows count.
void free_row();

// Generic function to unset bit in provided array.
void unset_bit();

// Test if provided path matches 
// a file in filesystem.
void is_file();

// Test if provided path matches 
// a directory in filesystem.
void is_dir();

// Test if provided path matches 
// a file or a directory in filesystem.
void exists();

// Find file by path and return its inode.
void find_inode_by_path();

// Find file by path in provided directory
// and return its inode.
void find_file_in_dir();

// Split path on strings.
void parse_path();

// Same as simplefs_creat without opening it.
void create_file();

// Same as simplefs_mkdir.
void create_dir();



#endif // SIMPLEFS_API_H