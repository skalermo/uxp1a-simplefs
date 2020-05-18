/*
 * test_inode.c
 *
 *      Author: Kordowski Mateusz
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 


#include "unity.h"


#include "superblock.h"
#include "open_files.h"
#include "block_links.h"
#include "dir_file.h"
#include "inode.h"

#include "simplefs_utils.h"

void* shm_addr = NULL;
const char* shm_name = "shm_test_init";

const uint32_t maxOpenFiles = 1024;
const uint32_t maxInodes = UINT16_MAX;
const uint32_t maxFilesystemSize = 33554432; //(32 MB)
const uint32_t sizeofOneBlock = 1024;

void setUp(void){

}

void tearDown(void){

}

void dispose_fs(void){
    munmap(shm_addr, maxFilesystemSize);
    shm_unlink(shm_name);
}

void create_fs(void){
    dispose_fs();

    // get shm fd
    int fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        puts("shm failed");
        exit(EXIT_FAILURE);
    }

    // allocate memory in shm 
    if (ftruncate(fd, maxFilesystemSize) == -1) {
        puts("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    // map the object into the caller's address space
    shm_addr = mmap(NULL, maxFilesystemSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_addr == MAP_FAILED) {
        puts("mmap failed");
        exit(EXIT_FAILURE);
    }

    struct Superblock toSet;

    toSet.max_number_of_inodes = maxInodes;
    toSet.max_number_of_open_files = maxOpenFiles;
    toSet.filesystem_checks = 0;
    toSet.data_block_size = sizeofOneBlock; 
    toSet.number_of_data_blocks = calculate_fs_needed_blocks(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    toSet.fs_size = maxFilesystemSize; 

    toSet.open_file_table_pointer = calculate_fs_superblock_end();
    toSet.open_file_bitmap_pointer = calculate_fs_open_file_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);

    toSet.inode_table_pointer = calculate_fs_open_file_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    toSet.inode_bitmap_pointer = calculate_fs_inode_table_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);

    toSet.block_links_pointer = calculate_fs_inode_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    toSet.block_bitmap_pointer = calculate_fs_block_links_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);
    toSet.data_blocks_pointer = calculate_fs_block_stat_end(maxOpenFiles, maxInodes, maxFilesystemSize, sizeofOneBlock);

    // create the most important structure
    fs_create_superblock_in_shm(&toSet, shm_addr);

    // create other structures
    fs_create_inode_structures_in_shm(shm_addr);
    fs_create_open_file_table_stuctures_in_shm(shm_addr);
    fs_create_blocks_stuctures_in_shm(shm_addr);

    // create last structure
    fs_create_main_folder(shm_addr);
}

uint8_t getNextInode(uint16_t prevInode, uint16_t* nextInode, char* partOfPath){
    uint32_t nextBlock;
    struct DirEntry copy;

    fs_get_data_from_inode_uint32(prevInode, 0, &nextBlock, shm_addr);

    uint32_t index = 0;
    uint8_t found = 0;

    for(; fs_get_dir_entry_copy(nextBlock, index, &copy, shm_addr) == 0; ++index){
         if(copy.inode_number != 0 && strcmp((char*) copy.name, partOfPath) == 0){
            found = 1;
            break;
        }
    }

    if(!found){
        return -1;
    }

    *nextInode = copy.inode_number;

    return 0;
}

uint8_t getFile(char* path){
    uint16_t currentInode = 1;
    char*** subPath = NULL;
    uint32_t numberOfStrings = parse_path(path, subPath);

    for(uint32_t i = 0; i < numberOfStrings; ++i){
        if(getNextInode(currentInode, &currentInode, (*subPath)[i]) == -1){
             puts("getFile - cannot find file - failed");
             return -1;
        }
    }

    // currentInode stores the inode to the file being searched
    return 0;
}

uint8_t createFile(char* path){

    uint16_t currentInode = 1;
    char*** subPath = NULL;
    uint32_t numberOfStrings = parse_path(path, subPath);
    uint32_t currentInodeBlock;

    for(uint32_t i = 0; i < numberOfStrings; ++i){
        if(getNextInode(currentInode, &currentInode, (*subPath)[i]) == -1){
            if(i == numberOfStrings - 1){
                // if the unknown name is the file we want to create
                uint16_t allocInode;
                struct Inode toSaveInode;
                struct DirEntry toSaveEntry;
                uint32_t indexOfNewEntry;

                toSaveEntry.inode_number = currentInode;
                strcpy((char*) toSaveEntry.name, (*subPath)[i]);
                toSaveEntry.name_len = 20; // dummy value, here find the length of this string

                toSaveInode.mode = 0;
                toSaveInode.readers = 0;
                toSaveInode.ref_count = 0;
                toSaveInode.file_size = 0;
                toSaveInode.block_index = fs_allocate_new_chain(shm_addr); // new chain of a new file. It can be set to FS_EMPTY_BLOCK_VALUE
                                                                            // if you dont want to allocate block
                if(toSaveInode.block_index == FS_EMPTY_BLOCK_VALUE) {
                    puts("createFile - cannot allocate new block - failed");
                    return -1;
                }

                if(fs_occupy_free_inode(&allocInode, &toSaveInode, shm_addr) != 0){
                    puts("createFile - cannot allocate innode - failed");
                    return -3;
                }

                // get block index of currentInode
                if(fs_get_data_from_inode_uint32(currentInode, 0, &currentInodeBlock, shm_addr) != 0){
                    puts("createFile - cannot get inode block- failed");
                    return -4;
                }
                
                // save the entry to the dir file of currentInode
                if(fs_occupy_free_dir_entry(currentInodeBlock, &indexOfNewEntry, &toSaveEntry, shm_addr) != 0){
                    puts("createFile - cannot get new dir entry- failed");
                    return -5;
                }

            }
            else {
                puts("createFile - cannot create file - failed");
                return -2;
            }
        }
    }

    return 0;
}

int main(void){
    UNITY_BEGIN();

    create_fs();    

    dispose_fs();
    
    return UNITY_END();
}

