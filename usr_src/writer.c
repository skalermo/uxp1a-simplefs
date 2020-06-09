#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "simplefs_api.h"


int main(int argc, char** argv) {
    if (argc != 3)
        exit(1);

    pid_t pid = getpid();
    int file_no = atoi(argv[1]);
    char *text = argv[2];

    int fd;
    if (file_no == 1) {
        fd = simplefs_open("/dir/file1", FS_WRITE);
    }
    else if (file_no == 2) {
        fd = simplefs_open("/dir/file2", FS_WRITE);
    } else
        exit(1);

    printf("\033[32m [Writer %d] opened file%d with fd=%d\n", pid, file_no, fd);
    int to_sleep;

    srand(time(NULL) - getpid());

    while(1) {
//        printf("Writer %d tries to write to file%d\n", pid, file_no);

        int bytes_written = simplefs_write(fd, text, strlen(text));
        simplefs_lseek(fd, SEEK_SET, 0);
        printf("\033[32m [Writer %d] wrote to file%d \033[37m'%.*s'\n", pid, file_no, bytes_written, text);

        to_sleep = (rand() % 5);
        sleep(to_sleep);
    }
}