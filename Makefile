CC          = gcc
CSTANDARD   = c99

INCLUDES    = -Iinclude -Isrc/mr_utils/include
LDLIBS      = -Lsrc/mr_utils/build/$(BUILD_TYPE) -lmr_utils -lm

WARNINGS  = -Wall -Wextra -Werror -Wpedantic -pedantic-errors
WARNINGS += -Wpointer-arith -Wcast-align -Wwrite-strings
WARNINGS += -Wstrict-prototypes
WARNINGS += -Wswitch-default -Wswitch-enum -Wunreachable-code
WARNINGS += -Wbad-function-cast -Wcast-qual -Wundef
WARNINGS += -Wshadow -Wfloat-equal -Wformat=2
WARNINGS += -Wredundant-decls -Wnested-externs

ifneq (,$(filter debug build-debug,$(MAKECMDGOALS)))
    BUILD_TYPE := debug
    CFLAGS     := -O0 -g -fno-omit-frame-pointer -rdynamic -DDEBUG -DMRD_DEBUG_ONLY_CALLED_AND_ERR $(WARNINGS) $(INCLUDES)
else
    BUILD_TYPE := release
    CFLAGS     := -O2 $(WARNINGS) $(INCLUDES)
endif

BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/$(BUILD_TYPE)

TARGET_TEST    = $(OBJ_DIR)/test.out
UTILS_SPACERS  = src/mr_utils/build/$(BUILD_TYPE)/spacers

SRC_LIB        = src/sgf.c
SRC_TEST_MAIN  = test/test.c

OBJ_LIB        = $(SRC_LIB:%.c=$(OBJ_DIR)/%.o)
OBJ_TEST_MAIN  = $(SRC_TEST_MAIN:%.c=$(OBJ_DIR)/%.o)

ALL_MAIN_OBJS   = $(OBJ_LIB)
ALL_TEST_OBJS  = $(OBJ_LIB) $(OBJ_TEST_MAIN)

.PHONY: all test run clean format format-check bear debug build-debug mr_utils_lib

all: $(TARGET_TEST)

mr_utils_lib:
	$(MAKE) -C src/mr_utils static-lib spacers BUILD_TYPE=$(BUILD_TYPE)

$(TARGET_TEST): mr_utils_lib $(ALL_TEST_OBJS)
	$(CC) $(ALL_TEST_OBJS) -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MD -c $< -o $@ -std=$(CSTANDARD) $(CFLAGS)

test: $(TARGET_TEST)
	./$(TARGET_TEST)

build-debug: $(TARGET_TEST)

debug: build-debug
	./$(TARGET_TEST)

clean:
	rm -rf $(BUILD_DIR)
	rm -f compile_commands.json
	$(MAKE) -C src/mr_utils clean

bear: clean
	bear -- make build-debug

format: mr_utils_lib
	find ./src ./test -name "*.c" -o -name "*.h" | xargs clang-format -i --verbose
	git ls-files --recurse-submodules | xargs $(UTILS_SPACERS)

format-check: mr_utils_lib
	find ./src ./test -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror --verbose
	git ls-files --recurse-submodules | xargs $(UTILS_SPACERS)

valgrind:
	valgrind --leak-check=full --suppressions=valgrind.supp $(TARGET_TEST)

record:
	perf record -g --call-graph dwarf $(TARGET_TEST)
	perf script > chombo.perf

-include $(ALL_MAIN_OBJS:.o=.d)
-include $(ALL_TEST_OBJS:.o=.d)
