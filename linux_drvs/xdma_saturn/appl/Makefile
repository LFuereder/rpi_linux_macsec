# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Iinc

# Directories
SRCDIR = src
INCDIR = inc
BUILDDIR = build
TARGET = $(BUILDDIR)/xdma_jobs

# Find all the source and header files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Include dependency files
-include $(DEPS)

# Clean up build directory and target
clean:
	rm -rf $(BUILDDIR) $(TARGET)

.PHONY: all clean new

new: clean all

