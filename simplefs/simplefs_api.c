#include "simplefs_api.h"
#include "simplefs_utils.h"
#include <libgen.h>
#include <string.h>

void* shm_addr = NULL;

int simplefs_open(char *name, int mode) {
    // Init system

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

    // Get Filename and dir path
    char* name_copy = strdup(name);

    char* filename = basename(name_copy);
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
    return ENOTIMPLEMENTED;
}


/**
 * To samo co w read tylko z sprawdzaniem, czy alokujemy nowe bloki, czy też nie.
 * Ważne przy synchronizacji.
 */
int simplefs_write(int fd, char *buf, int len) {
    return ENOTIMPLEMENTED;
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
    return ENOTIMPLEMENTED;
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