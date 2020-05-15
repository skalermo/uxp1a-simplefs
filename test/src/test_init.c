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

int main(void){
    UNITY_BEGIN();

    RUN_TEST(test_get_ptr_to_fs);
    RUN_TEST(test_mmap_fs);
    RUN_TEST(test_create_fs);

    return UNITY_END();
}
