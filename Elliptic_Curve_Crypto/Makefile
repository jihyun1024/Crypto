# Makefile for for_test.c

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g3
LDFLAGS = 

# Directories
SRC_DIR = .
CORE_DIR = core
OUTPUT_DIR = output

# Source files
SRCS = $(SRC_DIR)/for_test.c $(CORE_DIR)/sha256.c
OBJS = $(OUTPUT_DIR)/for_test.o $(OUTPUT_DIR)/sha256.o
TARGET = $(OUTPUT_DIR)/for_test.exe

# Include directories
INCLUDES = -I$(SRC_DIR)

# Default target
all: $(TARGET)

# Create output directory
$(OUTPUT_DIR):
	@mkdir -p $(OUTPUT_DIR)

# Link the executable
$(TARGET): $(OBJS) | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile for_test.c
$(OUTPUT_DIR)/for_test.o: $(SRC_DIR)/for_test.c | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC_DIR)/for_test.c -o $(OUTPUT_DIR)/for_test.o

# Compile sha256.c
$(OUTPUT_DIR)/sha256.o: $(CORE_DIR)/sha256.c | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(CORE_DIR)/sha256.c -o $(OUTPUT_DIR)/sha256.o

# Run the executable
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	@rm -f $(OUTPUT_DIR)/*.o $(TARGET)
	@echo "Cleaned build artifacts"

# Clean everything including output directory
distclean: clean
	@rm -rf $(OUTPUT_DIR)
	@echo "Cleaned all"

# Phony targets
.PHONY: all run clean distclean
