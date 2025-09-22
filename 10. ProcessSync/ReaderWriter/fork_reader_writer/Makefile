# Makefile for Reader-Writer Synchronization Programs

CC = g++
CFLAGS = -Wall -Wextra -std=c++11
TARGET_FORK = fork_reader_writer
TARGET_READER = reader
TARGET_WRITER = writer

# Default target
all: $(TARGET_FORK) $(TARGET_READER) $(TARGET_WRITER)

# Fork-based reader-writer program
$(TARGET_FORK): $(TARGET_FORK).cpp
	$(CC) $(CFLAGS) -o $(TARGET_FORK) $(TARGET_FORK).cpp

# Separate reader program
$(TARGET_READER): $(TARGET_READER).cpp
	$(CC) $(CFLAGS) -o $(TARGET_READER) $(TARGET_READER).cpp

# Separate writer program
$(TARGET_WRITER): $(TARGET_WRITER).cpp
	$(CC) $(CFLAGS) -o $(TARGET_WRITER) $(TARGET_WRITER).cpp

# Run the fork-based program
run-fork: $(TARGET_FORK)
	./$(TARGET_FORK)

# Run separate reader-writer programs (run writer first, then readers)
run-separate: $(TARGET_READER) $(TARGET_WRITER)
	@echo "Starting writer in background..."
	./$(TARGET_WRITER) &
	@sleep 1
	@echo "Starting readers..."
	./$(TARGET_READER) & ./$(TARGET_READER) & ./$(TARGET_READER) &
	@echo "All processes started. Wait for completion..."

# Clean compiled files
clean:
	rm -f $(TARGET_FORK) $(TARGET_READER) $(TARGET_WRITER)
	rm -f *.out

# Clean system resources (use if programs exit unexpectedly)
clean-ipc:
	@echo "Cleaning up IPC resources..."
	@ipcs -m | grep $(USER) | awk '{print $$2}' | xargs -r ipcrm -m
	@ipcs -s | grep $(USER) | awk '{print $$2}' | xargs -r ipcrm -s
	@echo "IPC cleanup complete."

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Compile all programs"
	@echo "  fork_reader_writer - Compile fork-based program"
	@echo "  reader       - Compile reader program"
	@echo "  writer       - Compile writer program"
	@echo "  run-fork     - Run fork-based reader-writer program"
	@echo "  run-separate - Run separate reader-writer programs"
	@echo "  clean        - Remove compiled files"
	@echo "  clean-ipc    - Clean up IPC resources (use if programs crash)"
	@echo "  help         - Show this help message"

.PHONY: all run-fork run-separate clean clean-ipc help
