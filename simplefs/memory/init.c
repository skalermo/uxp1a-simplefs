#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "init.h"
#include "superblock.h"
#include "inode.h"
#include "open_files.h"
#include "dir_file.h"
#include "block_links.h"


void *PTR_TO_FS = NULL;

void *get_ptr_to_fs() {
	return get_ptr_to_fs_custom(FS_SHM_NAME, FS_SIZE);
}

void *get_ptr_to_fs_custom(const char *path, const unsigned fs_size) {

	// simplefs is already mmapped
	if (PTR_TO_FS != NULL) {
		return PTR_TO_FS;
	}

	PTR_TO_FS = mmap_fs_custom(path, fs_size);

	while (PTR_TO_FS == NULL) {
		// mmap failed, create fs
		create_fs_custom(path, fs_size);

		// at this point fs should be created, mmap it
		PTR_TO_FS = mmap_fs_custom(path, fs_size);
	}

	return PTR_TO_FS;
}

void *mmap_fs() {
	return mmap_fs_custom(FS_SHM_NAME, FS_SIZE);
}

void *mmap_fs_custom(const char *path, const unsigned fs_size) {
	int fd = shm_open(path, O_RDWR, 0);

	// simplefs is not created yet
	if (fd == -1 && errno == ENOENT)
		return NULL;

	// some other error, just leave
	if (fd == -1 && errno != ENOENT) {
		perror("shm_open");
		exit(EXIT_FAILURE);
	}

	// we got a valid fd, mmap it
	void *addr = mmap(NULL, fs_size, 
					  PROT_READ | PROT_WRITE,
					  MAP_SHARED, fd, 0);

	if (addr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	return addr;
}

void create_fs() {
	create_fs_custom(FS_SHM_NAME, FS_SIZE);
}

void create_fs_custom(const char *path, const unsigned fs_size) {
	sem_t *sem = sem_open(CREATE_FS_GUARD, O_CREAT, 0644, 1);

	if (sem == SEM_FAILED) {
		perror("sem_open");
		exit(EXIT_FAILURE);
	}

	sem_wait(sem);

	int fd = shm_open(path,
					  O_CREAT | O_EXCL | O_RDWR,
					  S_IRUSR | S_IWUSR);

	// already exists, nothing to be done
	if (fd == -1 && errno == EEXIST) {
		sem_post(sem);
		sem_close(sem);
		return;
	}

	// some other error, just leave
	if (fd == -1 && errno != EEXIST) {
		perror("shm_open_creat_excl");
	}

	if (ftruncate(fd, fs_size) == -1) {
        puts("ftruncate failed");
        exit(EXIT_FAILURE);
    }

	void *addr = mmap(NULL, fs_size,
					  PROT_WRITE,
					  MAP_PRIVATE, fd, 0);

	if (addr == MAP_FAILED) {
	   perror("mmap");
	   exit(EXIT_FAILURE);
	}

    close(fd);

	// writing structures to shm

	struct Superblock sblock;
    sblock.max_number_of_inodes = MAX_INODES - 1;
    sblock.max_number_of_open_files = MAX_OPEN_FILES;
    sblock.filesystem_checks = 0;
    sblock.data_block_size = BLOCK_SIZE; 

    sblock.number_of_data_blocks = calculate_fs_needed_blocks(MAX_OPEN_FILES, 
    														  MAX_INODES, 
    														  fs_size, 
    														  BLOCK_SIZE);
    sblock.fs_size = fs_size; 

    sblock.open_file_table_pointer = calculate_fs_superblock_end();
    sblock.open_file_bitmap_pointer = calculate_fs_open_file_table_end(MAX_OPEN_FILES,
    														 MAX_INODES,
    														 fs_size,
    														 BLOCK_SIZE);

    sblock.inode_table_pointer = calculate_fs_open_file_stat_end(MAX_OPEN_FILES,
    															 MAX_INODES,
    															 fs_size,
    															 BLOCK_SIZE);

    sblock.inode_bitmap_pointer = calculate_fs_inode_table_end(MAX_OPEN_FILES, 
    														   MAX_INODES,
    														   fs_size,
    														   BLOCK_SIZE);

    sblock.block_links_pointer = calculate_fs_inode_stat_end(MAX_OPEN_FILES,
    														 MAX_INODES,
    														 fs_size,
    														 BLOCK_SIZE);

    sblock.block_bitmap_pointer = calculate_fs_block_links_end(MAX_OPEN_FILES, 
    														   MAX_INODES,
    														   fs_size,
    														   BLOCK_SIZE);

    sblock.data_blocks_pointer = calculate_fs_block_stat_end(MAX_OPEN_FILES, 
    														 MAX_INODES,
    														 fs_size, 
    														 BLOCK_SIZE);

    // create the most important structure
    fs_create_superblock_in_shm(&sblock, addr);

    // create other structures
    fs_create_inode_structures_in_shm(addr);

    fs_create_open_file_table_stuctures_in_shm(addr);

    fs_create_blocks_stuctures_in_shm(addr);

    // create last structure
    fs_create_main_folder(addr);

    munmap(addr, fs_size);

	// writing structures to shm // end

	sem_post(sem);
	sem_close(sem);

	// because fs was created
	// no more need in this semaphore
	sem_unlink(CREATE_FS_GUARD);
	// PTR_TO_FS = addr;
}

void unlink_fs() {
	unlink_fs_custom(FS_SHM_NAME);
}

void unlink_fs_custom(const char *path) {
	if (shm_unlink(path) == -1) {
		perror("shm_unlink");
		exit(EXIT_FAILURE);
	};
}

int get_data_blocks_number() {
    // calculate space for datablocks and divide by size of one block
    int size_of_all_structures = 0;
    int data_space = FS_SIZE - size_of_all_structures;
    return data_space / BLOCK_SIZE;
}

int get_superblock_size() {
    return sizeof(struct Superblock);
}

int get_open_file_table_size() {
    return sizeof(struct OpenFile) * MAX_OPEN_FILES;
}

int get_open_file_bitmap_size() {
    return MAX_OPEN_FILES / 8;  // divide by 8 to get result in bytes
}

int get_inode_table_size () {
    return sizeof(struct Inode) * MAX_INODES;
}
uint32_t get_InodeStat_size(uint32_t inode_count){
    // ceil(inode_count/8)
    uint32_t bitmaps = inode_count / 8 + (inode_count % 8 != 0);

    // InodeStat struct:
    // uint8_t* inode_bitmap;
    // uint16_t inode_used;

    return sizeof(uint8_t) * bitmaps + sizeof(uint16_t);
}

uint32_t get_BlockStat_size(uint32_t block_count){
    // ceil(block_count/8)
    uint32_t bitmaps = block_count / 8 + (block_count % 8 != 0);

    // BlockStat{
    // uint8_t* block_bitmap;
    // uint32_t used_data_blocks;}

    return sizeof(uint8_t) * bitmaps + sizeof(uint32_t);
}

uint32_t get_block_links_size(uint32_t block_count, uint32_t block_size){
    // TODO: Finish this
    // BlockLinks struct:
    // uint32_t* block_num;

    // needed blocks * sizeof(uint32_t)
    return block_count
}