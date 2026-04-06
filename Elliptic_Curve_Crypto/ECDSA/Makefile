CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LDFLAGS = 

# Source files
SRCS = main.c big_int.c ecc_point.c ecdsa.c sha256.c
OBJS = $(SRCS:.c=.o)
EXECUTABLE = ecdsa_demo

# Default target
all: $(EXECUTABLE)

# Link executable
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(EXECUTABLE)

# Run the program
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Phony targets
.PHONY: all clean run
