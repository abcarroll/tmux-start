# Makefile for tmux-start

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -O2
TARGET := tmux-start
SRC := tmux-start.c

# Default build rule
all: $(TARGET)

# Rule to build the binary
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Install rule
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

# Clean rule
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all install clean
