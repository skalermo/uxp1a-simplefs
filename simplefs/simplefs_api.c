#include "simplefs_api.h"
#include "simplefs_utils.h"
#include "memory/init.h"
#include <libgen.h>
#include <string.h>

void* shm_addr = NULL;

int simplefs_open(char *name, int mode) {
    // Init system
    shm_addr = get_ptr_to_fs();

    // Get Inode idx for file
    int inode_idx = get_inode_index(name, shm_addr);

    if(inode_idx < 0)
        return ENOTDIR;

    // Create Open File entry
    struct OpenFile new_open_file;
    new_open_file.mode = mode;
    new_open_file.inode_num = inode_idx;
    new_open_file.offset = 0;

    // Save Open file entry in FS
    int32_t fd = save_new_OpenFile_entry(&new_open_file, shm_addr);

    return fd;
}

int simplefs_creat(char *name, int mode) {
    // Init system
    shm_addr = get_ptr_to_fs();

    char* name_copy = strdup(name);

    // Get Filename and dir path
    char* filename = basename(name);
    char* dir_path = dirname(name_copy);

    // Get Inode idx for dir
    int dir_inode = get_inode_index(dir_path, shm_addr);

    if(dir_inode < 0)
        return ENOTDIR;

    // Create inode
    struct Inode new_inode = {0};
    new_inode.block_index = allocate_new_chain(shm_addr);

    if(new_inode.block_index < 0)
        return EIO;

    // Save inode in FS
    int32_t inode_idx = save_new_inode(&new_inode, shm_addr);

    if(inode_idx < 0)
        return ENOSPC;

    // Create Dir Entry
    struct DirEntry new_dir_entry;
    new_dir_entry.inode_number = inode_idx;
    strcpy(new_dir_entry.name, filename);
    new_dir_entry.name_len = strlen(filename);

    // Get directory block index
    uint32_t dir_block = get_inode_block_index(dir_inode, shm_addr);
    if(dir_block == INT32_MAX)
        return ENOENT;

    // Save dir entry in FS
    int32_t dir_entry_idx = save_new_dir_entry(dir_block, &new_dir_entry, shm_addr);
    if(dir_entry_idx < 0)
        return ENOSPC;


    // Create Open File entry
    struct OpenFile new_open_file;
    new_open_file.mode = mode;
    new_open_file.inode_num = inode_idx;
    new_open_file.offset = 0;

    // Save Open file entry in FS
    int32_t fd = save_new_OpenFile_entry(&new_open_file, shm_addr);

    free(name_copy);
    return fd;
}

int simplefs_read(int fd, char *buf, int len) {
    return ENOTIMPLEMENTED;
}

int simplefs_write(int fd, char *buf, int len) {
    return ENOTIMPLEMENTED;
}

int simplefs_lseek(int fd, int whence, int offset) {
    return ENOTIMPLEMENTED;
}

int simplefs_unlink(char *name) {
    return ENOTIMPLEMENTED;
}

int simplefs_mkdir(char *name) {
    return ENOTIMPLEMENTED;
}

int simplefs_rmdir(char *name) {
    return ENOTIMPLEMENTED;
}

int simplefs_close(int fd) {
    return ENOTIMPLEMENTED;
}