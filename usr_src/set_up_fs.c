#include <stdio.h>
#include "simplefs_api.h"


int main() {
    simplefs_mkdir("/dir");
    puts("Created /dir");
    int fd = simplefs_creat("/dir/file1", 0);
    puts("Created /dir/file1");
    simplefs_close(fd);
    fd = simplefs_creat("/dir/file2", 0);
    puts("Created /dir/file2");
    simplefs_close(fd);
}
