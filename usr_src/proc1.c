#include <stdio.h>
#include <string.h>
#include <memory/superblock.h>
#include "simplefs_api.h"



int main(int argc, char const *argv[])
{
    // Dir creation
    simplefs_mkdir("/dir1");
    simplefs_mkdir("/dir2");
    simplefs_mkdir("/dir1/dir3");
    puts("Created dirs");

    // File creation
    puts("Created /dir2/file2.txt");
    int fd = simplefs_creat("/dir2/file2.txt", WRONLY);
    simplefs_close(fd);

    // Write
    puts("Created /dir1/dir3/file.txt");
    fd = simplefs_creat("/dir1/dir3/file.txt", WRONLY);
    char text[] = "Hello world";
    puts("Writing to file");
    simplefs_write(fd, text, sizeof(text));
    simplefs_close(fd);

    // Read
    puts("Reading file");
    fd = simplefs_open("/dir1/dir3/file.txt", RDONLY);
    char buf[20];
    simplefs_read(fd, buf, sizeof(buf));

    if(!strcmp(buf, text)){
        puts("Correct Read");
    } else {
        puts("Incorrect Read");
    }
    simplefs_close(fd);

    puts("Writing and reading from same file");
    fd = simplefs_open("/dir2/file2.txt", RDWR);
    char text2[] = "New text";
    simplefs_write(fd, text2, sizeof(text2));

    // lseek
    simplefs_lseek(fd, SEEK_SET, 0);

    char buf2[20];
    simplefs_read(fd, buf2, sizeof(buf2));

    if(!strcmp(buf2, text2)){
        puts("Correct Read");
    } else {
        puts("Incorrect Read");
    }
    simplefs_close(fd);

    simplefs_rmdir("/dir2");

    fd = simplefs_creat("/file.bin", RDWR);
    simplefs_close(fd);

    simplefs_unlink("/file.bin");

    return 0;
}