#ifndef SIMPLEFS_UTILS_H
#define SIMPLEFS_UTILS_H

#include "simplefs_api.h"
#include "memory/block_links.h"
#include "memory/dir_file.h"
#include "memory/inode.h"
#include "memory/open_files.h"
#include "memory/utils.h"
#include "simplefs_synchronization.h"
#include <stdint.h>
#include <stdio.h>

#include "memory/superblock.h"


//define parse_path error
#define EINPATH -150

// Split path on strings.
//Return count of subpaths or EINPATH error
int parse_path(char* path, char*** subpath);

// Level 2 functions

int32_t next_inode(uint16_t prev_inode, char* name, uint8_t type, void* shm_addr);

/**
 * @param path
 * @return Inode index or Error code
 */
int32_t get_inode_index(char *path, uint8_t type, void* shm_addr);

/**
 * @brief synchronised fs_allocate_new_chain(void* addr)
 * @param shm_addr FS address
 * @return First block index or Error code
 */
uint32_t allocate_new_chain(void* shm_addr);

/**
 * @brief fs_occupy_free_inode function
 * @param inode Inode structure to save
 * @param shm_addr FS address
 * @return New inode index or Error code
 */
int32_t save_new_inode(struct Inode* inode, void* shm_addr);

/***
 * @brief fs_occupy_free_dir_entry function
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
 * @brief read
 * @param block_num First block num
 * @param offset
 * @param buf Read buffer
 * @param len Read length
 * @param shm_addr FS address
 * @return 0 or Error code
 */
int32_t read_buffer(uint32_t block_num, uint32_t offset, char* buf, int len, void* shm_addr);

/***
 * @brief write
 * @param block_num First block num
 * @param offset
 * @param buf Read buffer
 * @param len Read length
 * @param shm_addr FS address
 * @return 0 or Error code
 */
int32_t write_buffer(uint32_t block_num, uint32_t offset, char* buf, int len, void* shm_addr);

int is_dir_empty(uint16_t dir_inode, void *shm_addr);

// Getters for inode
struct Inode get_inode(uint16_t inode_idx, void* shm_addr);
uint32_t get_inode_block_index(uint16_t inode_idx, void* shm_addr);
uint16_t get_inode_file_size(uint16_t inode, void* shm_addr);
uint16_t get_inode_readers(uint16_t inode, void* shm_addr);
uint16_t get_inode_writers(uint16_t inode, void* shm_addr);
uint8_t  get_inode_mode(uint16_t inode, void* shm_addr);
uint8_t  get_ref_count(uint16_t inode, void* shm_addr);

// Setter for inode
int8_t set_inode_file_size(uint16_t inode, uint16_t filesize, void* shm_addr);

// Increment and decrement functions for inode
void inc_ref_count(uint16_t inode, void* shm_addr);

// Get for OpenFile
struct OpenFile get_open_file(uint32_t fd, void* shm_addr);

// Setter for OpenFile
int8_t set_offset(uint16_t fd, uint32_t offset, void* shm_addr);



#endif // SIMPLEFS_API_H
