# Advanced IPC with Poll() Demonstration

This directory contains a comprehensive C++ program that demonstrates the `poll()` system call working alongside multiple Inter-Process Communication (IPC) mechanisms.

## Files in this Directory

- `ipc_advanced_demo.cpp` - Main demonstration program
- `p1.cpp`, `p2.cpp` - Legacy simple poll examples
- `Makefile` - Build configuration
- `README.md` - This documentation

## Program Overview

The advanced demo creates a multi-process communication system where:

1. **Main Process (Consumer)**: Uses `poll()` to monitor multiple file descriptors simultaneously
2. **Producer Processes**: Generate data through different IPC mechanisms
3. **Coordination**: All communication is coordinated through the poll-based event loop

## IPC Mechanisms Demonstrated

### 1. Poll System Call
- **Purpose**: I/O multiplexing - monitor multiple file descriptors simultaneously
- **Benefits**: Non-blocking, efficient, event-driven programming
- **Usage**: Monitors pipes, FIFOs, and stdin in a single event loop

```cpp
struct pollfd poll_fds[MAX_FDS];
poll_fds[0].fd = file_descriptor;
poll_fds[0].events = POLLIN;  // Monitor for input data
int result = poll(poll_fds, num_fds, timeout_ms);
```

### 2. Anonymous Pipes
- **Purpose**: Communication between parent and child processes
- **Features**: Unidirectional, fast, kernel-buffered
- **Demo**: Multiple pipe producers send data to the main consumer

### 3. Named Pipes (FIFOs)
- **Purpose**: Communication between unrelated processes
- **Features**: Persistent