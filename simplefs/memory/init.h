#ifndef SIMPLEFS_INIT_H
#define SIMPLEFS_INIT_H

#include <stdint.h>

#define FS_NAME "simplefs_shm"
#define FS_SIZE 268435456 // 256 MiB
#define MAX_INODES 65536 // 2^16
#define MAX_OPEN_FILES 1024
#define BLOCK_SIZE 1024 // 1 KiB

// name of the semaphore synchronizing fs creation
#define CREATE_FS_GUARD "create_fs_guard"

// global pointer to mapped fs
extern void *PTR_TO_FS;

// Here are default functions with no arguments
// and *_custom alternatives where you can specify arguments.
// Default functions simply call custom ones with default arguments.
// (Because no function overloading for you in vanilla C)

// get pointer to simplefs
// function calls mmap within itself
// so returned pointer is ready to use
void *get_ptr_to_fs();

void *get_ptr_to_fs_custom(const char *path, unsigned fs_size);

void *mmap_fs();

void *mmap_fs_custom(const char *path, unsigned fs_size);

// create and initialize fs inside shm
// default parameters
void create_fs();

// alternative option for testing
void create_fs_custom(const char *path, unsigned fs_size);

// unlink shared memory bound to fs
void unlink_fs();

void unlink_fs_custom(const char *path);

uint32_t get_data_block_count(uint32_t fs_size, uint32_t size_without_blocks, uint32_t block_size);

int get_superblock_size();

int get_open_file_table_size(uint32_t open_file_count);

int get_open_file_stat_size(uint32_t open_file_count);

int get_inode_table_size (uint32_t inode_count);

uint32_t get_InodeStat_size(uint32_t inode_count);

uint32_t get_BlockStat_size(uint32_t block_count);

uint32_t get_block_links_size(uint32_t block_count);





# endif // SIMPLEFS_INIT_H