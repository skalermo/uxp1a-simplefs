#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "unity.h"
#include "init.h"

void setUp(void) {

}

void tearDown(void) {

}


void test_get_ptr_to_fs() {
	const char *path = "test_fs";
	const unsigned fs_size = 4194304;  // 4 MiB
	void *ptr = NULL;

	ptr = get_ptr_to_fs_custom(path, fs_size);
	TEST_ASSERT_NOT_EQUAL(NULL, ptr);

	unlink_fs_custom(path);

}

void test_mmap_fs() {

	const char *path = "test_fs";
	const unsigned fs_size = 4194304;  // 4 MiB
	void *ptr;

	// make sure fs doesn't exist at the beginning
	// unlink_fs_custom(path);

	// there is no fs, so mmap should fail
	ptr = mmap_fs_custom(path, fs_size);
	TEST_ASSERT_EQUAL(NULL, ptr);

	create_fs_custom(path, fs_size);

	// now it shouldn't fail
	ptr = mmap_fs_custom(path, fs_size);
	TEST_ASSERT_NOT_EQUAL(NULL, ptr);

	munmap(ptr, fs_size);
	unlink_fs_custom(path);

}

void test_create_fs() {

	const char *path = "test_fs";
	const unsigned fs_size = 4194304;  // 4 MiB
	int fd;

	// make sure fs doesn't exist at the beginning
	// unlink_fs_custom(path);

	// check if fs doesn't exist
	fd = shm_open(path,
					  O_RDWR,
					  S_IRUSR | S_IWUSR);

	TEST_ASSERT_EQUAL(-1, fd);
	TEST_ASSERT_EQUAL(ENOENT, errno);

	create_fs_custom(path, fs_size);

	// now check if it exists
	fd = shm_open(path,
					  O_RDWR,
					  S_IRUSR | S_IWUSR);

	TEST_ASSERT_TRUE(fd >= 0);
	close(fd);


	// also check if it appeared in /dev/shm
	char full_path[80];
	strcpy(full_path, "/dev/shm/");
	strcat(full_path, path);
	TEST_ASSERT_EQUAL(0, access(full_path, F_OK ));
	unlink_fs_custom(path);

	// now check if it doesn't exist
	fd = shm_open(path,
				  	  O_RDWR,
				  	  S_IRUSR | S_IWUSR);

	TEST_ASSERT_EQUAL(-1, fd);
	TEST_ASSERT_EQUAL(ENOENT, errno);
	TEST_ASSERT_EQUAL(-1, access(full_path, F_OK ));
}

void test_getters() {
    const uint32_t inode_count = 65536;
    const uint32_t open_file_count = 1024;
    const uint32_t fs_size = 268435456;             // 256 MiB
    const uint32_t block_size = 1024;               // 1 KiB
    uint32_t total_size = 0;

    printf("%u\n", get_superblock_size()); // 44
    total_size += get_superblock_size();

    printf("%u\n", get_InodeStat_size(inode_count)); // 8194
    total_size += get_InodeStat_size(inode_count);

    printf("%u\n", get_inode_table_size(inode_count)); // 1048576
    total_size += get_inode_table_size(inode_count);

    printf("%u\n", get_open_file_stat_size(open_file_count)); // 130
    total_size += get_open_file_stat_size(open_file_count);

    printf("%u\n", get_open_file_table_size(open_file_count)); // 8192
    total_size += get_open_file_table_size(open_file_count);

    const uint32_t size_without_blocks = total_size;
    const uint32_t block_count = get_data_block_count(fs_size, size_without_blocks, block_size);

    printf("%u\n", block_count);

    printf("%u\n", get_BlockStat_size(block_count));
    total_size += get_BlockStat_size(block_count);

    printf("%u\n", get_block_links_size(block_count));
    total_size += get_block_links_size(block_count);

    total_size += block_count * block_size;

    TEST_ASSERT_TRUE(total_size <= fs_size);
}

int main(void){
    UNITY_BEGIN();

    RUN_TEST(test_get_ptr_to_fs);
    RUN_TEST(test_mmap_fs);
    RUN_TEST(test_create_fs);
    RUN_TEST(test_getters);

    return UNITY_END();
}
