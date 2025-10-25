# Makefile for Advanced IPC Poll Demo

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -pthread
TARGET = ipc_advanced_demo
SOURCE = ipc_advanced_demo.cpp

# Legacy targets for existing programs
LEGACY_TARGETS = p1 p2

.PHONY: all clean run demo legacy help

# Default target
all: $(TARGET)

# Build the advanced IPC demo
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

# Build legacy programs
legacy: $(LEGACY_TARGETS)

p1: p1.cpp
	$(CXX) $(CXXFLAGS) -o p1 p1.cpp

p2: p2.cpp
	$(CXX) $(CXXFLAGS) -o p2 p2.cpp

# Run the advanced demo
demo: $(TARGET)
	@echo "=== Running Advanced IPC Poll Demo ==="
	@echo "Commands while running:"
	@echo "  status  - Show IPC status"
	@echo "  signal  - Send SIGUSR1 to self"
	@echo "  quit    - Exit program"
	@echo "  Ctrl+C  - Send SIGINT"
	@echo ""
	./$(TARGET)

# Run legacy demo
run: p1 p2
	@echo "=== Running Legacy Poll Demo ==="
	./p1

# Clean all built files and temporary resources
clean:
	rm -f $(TARGET) $(LEGACY_TARGETS)
	rm -f /tmp/demo_fifo
	@echo "Cleaned all build files and temporary resources"

# Show help
help:
	@echo "Available targets:"
	@echo "  all     - Build the advanced IPC demo (default)"
	@echo "  demo    - Build and run the advanced IPC demo"
	@echo "  legacy  - Build legacy p1 and p2 programs"
	@echo "  run     - Build and run legacy demo"
	@echo "  clean   - Remove all built files and temp resources"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "Advanced Demo Features:"
	@echo "  - poll() system call demonstration"
	@echo "  - Multiple anonymous pipes"
	@echo "  - Named pipes (FIFOs)"
	@echo "  - Message queues"
	@echo "  - Shared memory"
	@echo "  - Signal handling"
