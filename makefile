#############################
# FLAGS
#############################

CC := gcc
CCFLAGS := -Wall -g  # More flags to be added...
INCLUDE_FS = -I $(FS_DIR)
INCLUDE_MEM_FS = -I $(FS_MEM_DIR)
VALGRIND = 


#############################
# FIXED FOLDERS AND FILES
#############################

FS_DIR := simplefs
FS_MEM_DIR := $(FS_DIR)/memory
LIB_DIR := lib
USR_SRC_DIR := usr_src
TEST_DIR := test
TEST_SRC_DIR := $(TEST_DIR)/src
FS_SRCS := $(wildcard $(FS_DIR)/*.c)
FS_MEM_SRCS := $(wildcard $(FS_MEM_DIR)/*.c)

USR_SRCS := $(wildcard $(USR_SRC_DIR)/*.c)
TEST_SRCS := $(wildcard $(TEST_SRC_DIR)/test_*.c)



#############################
# GENERATED FOLDERS AND FILES
#############################

BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/obj

TEST_BUILD_DIR := $(TEST_DIR)/build
TEST_BIN_DIR := $(TEST_BUILD_DIR)/bin
TEST_OBJ_DIR := $(TEST_BUILD_DIR)/obj
TEST_LOG := $(TEST_DIR)/test_log.txt

UNITY_DIR := $(TEST_DIR)/Unity
UNITY_SRC_DIR := $(UNITY_DIR)/src

BUILD_PATHS := $(BUILD_DIR) $(BIN_DIR) $(OBJ_DIR)

TEST_BUILD_PATHS := $(UNITY_DIR) $(TEST_BUILD_DIR)\
					$(TEST_BIN_DIR) $(TEST_OBJ_DIR)



#############################
# TARGETS
#############################

USR_TARGETS := $(patsubst $(USR_SRC_DIR)/%.c,$(BIN_DIR)/%.out,$(USR_SRCS))
LIB_TARGET := simplefs_lib.so


all: $(BUILD_PATHS) $(USR_TARGETS)

lib: $(LIB_DIR)/$(LIB_TARGET)

$(BIN_DIR)/%.out: $(OBJ_DIR)/%.o $(LIB_DIR)/$(LIB_TARGET)
	$(CC) $^ -o $@ -L$(LIB_DIR) -lrt

$(OBJ_DIR)/%.o: $(USR_SRC_DIR)/%.c
	$(CC) $(CCFLAGS) $(INCLUDE_FS) $(INCLUDE_MEM_FS) -c $< -o $@

$(LIB_DIR)/$(LIB_TARGET): $(FS_SRCS) $(FS_MEM_SRCS)
	$(CC) -g $(INCLUDE_MEM_FS) -fPIC $^ -shared -o $(LIB_TARGET)
	mkdir -p $(LIB_DIR)
	mv $(LIB_TARGET) $(LIB_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)



#############################
# TEST TARGETS
#############################

TEST_TARGETS := $(patsubst $(TEST_SRC_DIR)/test_%.c,$(TEST_BIN_DIR)/test_%.out,$(TEST_SRCS))


test: build_tests run_tests

valgrind:
	$(eval VALGRIND := valgrind --leak-check=full)

build_tests: lib $(TEST_BUILD_PATHS) $(TEST_TARGETS)

run_tests: build_tests
	@echo >> $(TEST_LOG)
	@date "+%Y-%m-%d[%H:%M:%S]" >> $(TEST_LOG)
	@for test_exec in $(TEST_TARGETS); do echo $$test_exec; { $(VALGRIND) ./$$test_exec; } 2>&1 | tee -a $(TEST_LOG); done

$(TEST_BIN_DIR)/test_%.out: $(TEST_OBJ_DIR)/test_%.o $(TEST_OBJ_DIR)/unity.o $(LIB_DIR)/$(LIB_TARGET)
	$(CC) $(CCFLAGS) $^ -o $@ -L$(LIB_DIR) -lrt -pthread

$(TEST_OBJ_DIR)/test_%.o: $(TEST_SRC_DIR)/test_%.c
	$(CC) $(CCFLAGS) $(INCLUDE_FS) $(INCLUDE_MEM_FS) -I $(UNITY_SRC_DIR) -c $< -o $@

$(TEST_OBJ_DIR)/unity.o: $(UNITY_SRC_DIR)/unity.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(UNITY_SRC_DIR)/unity.c: $(UNITY_DIR)

$(UNITY_DIR):
	git clone https://github.com/ThrowTheSwitch/Unity.git
	mv Unity/ $(TEST_DIR)

$(TEST_BUILD_DIR):
	mkdir -p $(TEST_BUILD_DIR)

$(TEST_BIN_DIR):
	mkdir -p $(TEST_BIN_DIR)

$(TEST_OBJ_DIR):
	mkdir -p $(TEST_OBJ_DIR)



#############################
# MISC
#############################

clean:
	rm -f $(USR_TARGETS)
	rm -f $(TEST_TARGETS)
	rm -f $(LIB_DIR)/$(LIB_TARGET)

	rm -f $(wildcard $(OBJ_DIR)/*)
	rm -f $(wildcard $(TEST_OBJ_DIR)/*)

distclean:
	rm -rf $(LIB_DIR)
	rm -rf $(BUILD_PATHS)
	rm -rf $(TEST_BUILD_PATHS)
	rm -f $(TEST_LOG)


.SECONDARY:
.PHONY: all lib clean distclean
.PHONY: test build_tests run_tests print_last_run_results valgrind
