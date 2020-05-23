#ifndef SIMPLEFS_UTILS_H
#define SIMPLEFS_UTILS_H

#include "simplefs_internals.h"
#include "memory/block_links.h"
#include "memory/dir_file.h"
#include "memory/inode.h"
#include "memory/open_files.h"
#include "memory/utils.h"
#include <stdint.h>



//define parse_path error
#define EINPATH -150

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
void set_bit(uint8_t *bitmap, int index);

// Unset bit in inode_bitmap and
// decrement by 1 used blocks count.
void free_block();

// Unset bit in inode_bitmap and
// decrement by 1 used rows count.
void free_row();

// Generic function to unset bit in provided array.
void unset_bit(uint8_t *bitmap, int index);

// Test if provided path matches 
// a file in filesystem.
void is_file();

// Test if provided path matches 
// a directory in filesystem.
void is_dir();

// Test if provided path matches 
// a file or a directory in filesystem.
void exists();

// Find file by path in provided directory
// and return its inode.
void find_file_in_dir();

// Split path on strings.
//Return count of subpaths or EINPATH error
int parse_path(char* path, char*** subpath);

// Same as simplefs_creat without opening it.
void create_file();

// Same as simplefs_mkdir.
void create_dir();

// Level 2 functions
/**
 * @param path
 * @return Inode index or Error code
 */
int32_t get_inode_index(char *path, void* shm_addr);

/**
 * @brief synchronised fs_allocate_new_chain(void* addr)
 * @param shm_addr FS address
 * @return First block index or Error code
 */
uint32_t allocate_new_chain(void* shm_addr);

/**
 * @brief Synchronised fs_occupy_free_inode function
 * @param inode Inode structure to save
 * @param shm_addr FS address
 * @return New inode index or Error code
 */
int32_t save_new_inode(struct Inode* inode, void* shm_addr);

/***
 * @brief Synchronised fs_occupy_free_dir_entry function
 * @param dir_block_number Block number where struct dir_file
 * @param dir_entry DirEntry structure to save
 * @param shm_addr FS address
 * @return New dir entry index or Error code
 */
int32_t save_new_dir_entry(uint32_t dir_block_number, struct DirEntry* dir_entry, void* shm_addr);

/***
 * @brief Search bitmap for empty space and save OpenFile struct
 * @param open_file_entry OpenFile structure to save
 * @param shm_addr FS address
 * @return New OpenFile index or Error code
 */
int32_t save_new_OpenFile_entry(struct OpenFile* open_file_entry, void* shm_addr);

/***
 * @brief Free all block in chain
 * @param block_index First data block index
 * @param shm_addr FS address
 * @return 0 or Error code
 */
int16_t free_data_blocks(uint32_t block_index, void* shm_addr);

/***
 * 
 * @param inode Inode index
 * @param shm_addr FS address
 * @return 0 or Error code
 */
int16_t free_inode(uint16_t inode, void* shm_addr);

/***
 * 
 * @param dir_block_number Block number where struct dir_file
 * @param inode Inode index
 * @param shm_addr FS address
 * @return 0 or Error code
 */
int16_t free_dir_entry(uint32_t dir_block_number, uint16_t inode, void* shm_addr);

/***
 * @brief Set bit in bitmap
 * @param fd OpenFile index
 * @param shm_addr FS address
 * @return 0 or Error code
 */
int16_t free_OpenFile(uint32_t fd, void* shm_addr);

/***
 * @brief Synchronised read
 * @param block_num First block num
 * @param offset
 * @param buf Read buffer
 * @param len Read length
 * @param shm_addr FS address
 * @return 0 or Error code
 */
int16_t read_buffer(uint32_t block_num, uint32_t offset, char* buf, int len, void* shm_addr);

/***
 * @brief Synchronised write
 * @param block_num First block num
 * @param offset
 * @param buf Read buffer
 * @param len Read length
 * @param shm_addr FS address
 * @return 0 or Error code
 */
int16_t write_buffer(uint32_t block_num, uint32_t offset, char* buf, int len, void* shm_addr);


// Synchronised getters for inode_idx
struct Inode get_inode(uint16_t inode_idx, void* shm_addr);
uint32_t get_inode_block_index(uint16_t inode_idx, void* shm_addr);
uint16_t get_inode_file_size(uint16_t inode, void* shm_addr);
uint16_t get_inode_readers(uint16_t inode, void* shm_addr);
uint16_t get_inode_writers(uint16_t inode, void* shm_addr);
uint8_t  get_inode_mode(uint16_t inode, void* shm_addr);
uint8_t  get_ref_count(uint16_t inode, void* shm_addr);

// Synchronised setters for inode
void set_inode_block_index(uint16_t inode, uint32_t block_index, void* shm_addr);
void set_inode_file_size(uint16_t inode, uint16_t filesize, void* shm_addr);
void set_inode_mode(uint16_t inode, uint8_t mode, void* shm_addr);

// Synchronised increment and decrement functions for inode
void inc_ref_count(uint16_t inode, void* shm_addr);
void dec_ref_count(uint16_t inode, void* shm_addr);
void inc_inode_readers(uint16_t inode, void* shm_addr);
void dec_inode_readers(uint16_t inode, void* shm_addr);
void inc_inode_writers(uint16_t inode, void* shm_addr);
void dec_inode_writers(uint16_t inode, void* shm_addr);

// Synchronised get for OpenFile
struct OpenFile get_open_file(uint32_t fd, void* shm_addr);

// Synchronised setters for OpenFile
void set_inode_num(uint16_t fd, uint16_t inode_num, void* shm_addr);
void set_offset(uint16_t fd, uint32_t offset, void* shm_addr);

// Synchronised get for DirEntry
int8_t get_dir_entry(uint32_t dir_file_block, uint32_t entry_idx, struct DirEntry* return_entry, void* shm_addr);


#endif // SIMPLEFS_API_H
