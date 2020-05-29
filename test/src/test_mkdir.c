#include "unity.h"
#include "simplefs_utils.h"
#include "init.h"
#include <stdlib.h>

void setUp(void) {

}


void tearDown(void) {

}


void make_one_dir() {
    void *ptr = get_ptr_to_fs();
    uint32_t inode_count = fs_get_used_inodes(ptr);
    printf("%u\n", inode_count);

    int ret_val = simplefs_mkdir("a");
    inode_count = fs_get_used_inodes(ptr);
    printf("%u\n", inode_count);


}


int main(void) {


    UNITY_BEGIN();


    RUN_TEST(make_one_dir);


    return UNITY_END();
}