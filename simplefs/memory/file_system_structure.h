#ifndef FILE_SYSTEM_STRUCTURE_H
#define FILE_SYSTEM_STRUCTURE_H

#include "superblock.h"
#include "open_files.h"
#include "dir_file.h"
#include "inode.h"
#include "block_links.h"

const int createFileSystemStructure(const char* shm_name);


#endif