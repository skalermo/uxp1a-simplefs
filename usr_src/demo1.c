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
    char * buffer = 0;
    long length;



    FILE * f = fopen ("./usr_src/mountain.jpg", "rb");

    if (f)
    {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        rewind (f);
        buffer = malloc (length);
        if (buffer)
        {
            fread (buffer, 1, length, f);
        }
        fclose (f);
    }
    printf("%ld\n", length);
    int fd = simplefs_creat("/output.jpg", FS_WRITE);
    int result = 0;
    if (buffer)
    {
        result = simplefs_write(fd, buffer, length);
        if(result != length)
            printf("OH NO %d", result);
        printf("result %d\n", result);
    }

    free(buffer);
    simplefs_close(fd);

    char* result_buffer = malloc(length);
    int fd2 = simplefs_open("/output.jpg", FS_READ);
    int read_result = simplefs_read(fd2, result_buffer, result);
    printf("read result %d\n", read_result);
    FILE * dest = fopen("./usr_src/output.jpg", "wb");

    fwrite( result_buffer, 1, result, dest);
    fclose(dest);

    free(result_buffer);

    simplefs_close(fd2);
    simplefs_unlink("/output.jpg");
    unlink_fs();
    return 0;
}