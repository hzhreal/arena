# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Iinclude -std=c23
ifdef DEBUG
	CFLAGS += -g
else
	CFLAGS += -DNDEBUG
endif

# Source directory
SRC_DIR = source

# Include directory
INC_DIR = include

# Object directory
OBJ_DIR = obj

# Library directory
LIB_DIR = lib

# Test directory
TEST_DIR = test

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Header files
HDRS = $(wildcard $(INC_DIR)/*.h)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Library name
LIB_NAME = Arena

# Target
TARGET = $(LIB_DIR)/lib$(LIB_NAME).a

# Test source file
TEST_SRC = $(TEST_DIR)/test.c

# Test object file
TEST_OBJ = $(TEST_DIR)/test.o

# Test executable
TEST_EXEC = $(TEST_DIR)/test

# Rule to compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build library
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	ar rcs $@ $^

# Rule to build test executable
$(TEST_EXEC): $(TEST_OBJ)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJ) -L$(LIB_DIR) -l$(LIB_NAME)

.PHONY: all clean test

# Build all
all: $(TARGET)

# Install library
install:
	make
	sudo cp lib/$(LIB_NAME) /usr/local/lib
	for header in $(HDRS); do \
		sudo cp $$header /usr/local/include/; \
	done

# Build test executable
test: $(TEST_EXEC)

# Clean objects and library
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(TEST_OBJ) $(TEST_EXEC)

	if [ -f /usr/local/lib/$(LIB_NAME) ]; then \
		sudo rm /usr/local/lib/$(LIB_NAME); \
	fi

	for header in $(HDRS); do \
		header_name=$$(basename $$header); \
		if [ -f /usr/local/include/$$header_name ]; then \
			sudo rm /usr/local/include/$$header_name; \
		fi \
	done

	for header in $(HDRS); do \
		header_name=$$(basename $$header); \
		if [ -f /usr/local/include/$$header_name ]; then \
			sudo rm /usr/local/include/$$header_name; \
		fi \
	done
