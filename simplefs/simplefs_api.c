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

    // Check if exists
    int32_t inode_idx = -1;//get_inode_index(name, shm_addr);
    if(inode_idx < 0) {
        char *name_copy = strdup(name);

        // Get Filename and dir path
        char *filename = basename(name);
        char *dir_path = dirname(name_copy);

        // Get Inode idx for dir
        int dir_inode = get_inode_index(dir_path, shm_addr);

        if (dir_inode < 0)
            return ENOTDIR;

        // Create inode
        struct Inode new_inode = {0};
        new_inode.block_index = allocate_new_chain(shm_addr);

        if (new_inode.block_index < 0)
            return EIO;

        // Save inode in FS
        inode_idx = save_new_inode(&new_inode, shm_addr);

        if (inode_idx < 0)
            return ENOSPC;

        // Create Dir Entry
        struct DirEntry new_dir_entry;
        new_dir_entry.inode_number = inode_idx;
        strcpy(new_dir_entry.name, filename);
        new_dir_entry.name_len = strlen(filename);

        // Get directory block index
        uint32_t dir_block = get_inode_block_index(dir_inode, shm_addr);
        if (dir_block == INT32_MAX)
            return ENOENT;

        // Save dir entry in FS
        int32_t dir_entry_idx = save_new_dir_entry(dir_block, &new_dir_entry, shm_addr);
        if (dir_entry_idx < 0)
            return ENOSPC;

        free(name_copy);
    }

    // Create Open File entry
    struct OpenFile new_open_file;
    new_open_file.mode = mode;
    new_open_file.inode_num = inode_idx;
    new_open_file.offset = 0;

    // Save Open file entry in FS
    int32_t fd = save_new_OpenFile_entry(&new_open_file, shm_addr);

    return fd;
}

/**
 * Zmienia w inode mode, czeka, jeżeli mode jest niekompatybilny z tym, co chce zrobić
 * lub nie zmienia nic, bo już dany mode jest taki jaki chce (tylko mode do read)
 * 
 * Zmienia offset w open file
 * 
 * Po wyjściu zmienia mode w inode na 0, jeżeli nikt inny nie czyta. Jeżeli czytają, to nie zmieniamy.
 * Jeżeli write, to i tak inne read czekają, więc można mode wyzerować.
 */
int simplefs_read(int fd, char *buf, int len) {
    // Init system
    shm_addr = get_ptr_to_fs();

    struct OpenFile openFile = get_open_file(fd, shm_addr);
    if(openFile.mode != FS_READ){
        return EBADF;
    }

    // Set inode mode to READ
    set_inode_mode(openFile.inode_num, openFile.mode, shm_addr);

    // Get data block
    uint32_t block_idx = get_inode_block_index(openFile.inode_num, shm_addr);

    // Read Buffer
    read_buffer(block_idx, openFile.offset, buf, len, shm_addr);

    // Update offset
    openFile.offset += len;
    set_offset(fd, openFile.offset, shm_addr);

    // Set inode mode to 0
    set_inode_mode(openFile.inode_num, 0, shm_addr);

    return 0;
}


/**
 * To samo co w read tylko z sprawdzaniem, czy alokujemy nowe bloki, czy też nie.
 * Ważne przy synchronizacji.
 */
int simplefs_write(int fd, char *buf, int len) {
    // Init system
    shm_addr = get_ptr_to_fs();

    struct OpenFile openFile = get_open_file(fd, shm_addr);
    if(openFile.mode != FS_WRITE){
        return EBADF;
    }

    // Set inode mode to WRITE
    set_inode_mode(openFile.inode_num, openFile.mode, shm_addr);

    // Get data block
    uint32_t block_idx = get_inode_block_index(openFile.inode_num, shm_addr);

    // Write Buffer
    write_buffer(block_idx, openFile.offset, buf, len, shm_addr);

    // Update offset
    openFile.offset += len;
    set_offset(fd, openFile.offset, shm_addr);

    // Set inode mode to 0
    set_inode_mode(openFile.inode_num, 0, shm_addr);

    return 0;
}


/**
 * Zmiana offset w open file.
 * 
 * Sprawdzenie czy offset nie przekracza file size
 */
int simplefs_lseek(int fd, int whence, int offset) {
    return ENOTIMPLEMENTED;
}


/**
 * Oznaczenie w bitmapie jako pusty w inode stat.
 * 
 * Usunięcie w dir file struktury odwołującej się do danego inoda
 * 
 * Sprawdzenie ref count.
 */
int simplefs_unlink(char *name) {
    // Init system
    shm_addr = get_ptr_to_fs();

    // Get file inode
    int file_inode = get_inode_index(name, shm_addr);
    if(file_inode < 0){
        return file_inode;
    }

    // Check ref_count
    int ref_count = get_ref_count(file_inode, shm_addr);
    if(ref_count > 0)
        return EBUSY;

    // Get dir inode
    char* name_copy = strdup(name);
    char* dir_path = dirname(name_copy);
    int dir_inode = get_inode_index(dir_path, shm_addr);

    if(dir_inode < 0){
        return dir_inode;
    }

    // Get directory block index
    uint32_t dir_block = get_inode_block_index(dir_inode, shm_addr);
    if(dir_block == INT32_MAX)
        return ENOENT;

    // Remove dir entry from dir file
    int ret = free_dir_entry(dir_block, file_inode, shm_addr);
    if(ret < 0){
        return ret;
    }

    // Free blocks
    uint32_t file_block = get_inode_block_index(file_inode, shm_addr);
    ret = free_data_blocks(file_block, shm_addr);
    if(ret < 0){
        return ret;
    }

    // Set bit in bitmap
    ret = free_inode(file_inode, shm_addr);
    if(ret < 0){
        return ret;
    }

    return 0;
}


/**
 * Podobne do simplefs_creat, tylko zmieniamy dir file zamiast inodow
 */
int simplefs_mkdir(char *name) {
    return ENOTIMPLEMENTED;
}


/**
 * Podobne do simplefs_unlink, tylko zmieniamy dir file zamiast inodow
 * 
 */
int simplefs_rmdir(char *name) {
    return ENOTIMPLEMENTED;
}


/**
 * Usunięcie w open file.
 */
int simplefs_close(int fd) {
    return ENOTIMPLEMENTED;
}