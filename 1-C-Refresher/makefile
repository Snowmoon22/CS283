# Compiler settings
CC = gcc

# Target executable name
TARGET = stringfun

# Default target
all: $(TARGET)

# Compile source to executable
$(TARGET): stringfun.c
	$(CC) -o $(TARGET) $^

# Clean up build files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean