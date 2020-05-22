#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <malloc.h>
#include <sys/mman.h>
#include <memory/superblock.h>
#include <semaphore.h>
#include "simplefs_utils.h"
#include "simplefs_api.h"
#include "memory/init.h"
#include <sys/shm.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c\n"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

int main(int argc, char const *argv[])
{
    sem_unlink(CREATE_FS_GUARD);

    shm_unlink(FS_SHM_NAME);

//    create_fs();

    int fd = simplefs_creat("/file.txt", 1);
    void* shm_addr = get_ptr_to_fs();

    struct Superblock* superblock = shm_addr;

    struct OpenFileTable* oft = fs_get_open_file_table_ptr(shm_addr);
    uint8_t * inodeTable = fs_get_inode_table_ptr(shm_addr);
//    struct InodeStat* inodeStat= fs_get_inode_bitmap_ptr(shm_addr);
//
//    for(int i=0; i<8; ++i){
//        printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(inodeStat->inode_bitmap[i]));
//    }

    int idx = get_inode_index("/file.txt", shm_addr);

    int block_idx = get_inode_block_index(idx, shm_addr);
    int file_size = get_inode_file_size(idx, shm_addr);
    short readers = get_inode_readers(idx,shm_addr);
    short writers = get_inode_writers(idx,shm_addr);
    char mode = get_inode_mode(idx,shm_addr);
    char ref_count =  get_ref_count(idx,shm_addr);
    struct Inode inode = get_inode(idx, shm_addr);

    unlink_fs();
    return 0;
}