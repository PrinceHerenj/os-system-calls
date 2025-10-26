# Page Replacement Algorithms

This directory contains implementations of various page replacement algorithms used in operating systems for virtual memory management. Each algorithm is implemented in a separate C++ file with its own main function for testing and demonstration.

## Overview

Page replacement algorithms are crucial components of virtual memory systems. When physical memory is full and a new page needs to be loaded, these algorithms determine which existing page should be removed (replaced) to make room for the new page.

## Algorithms Implemented

### 1. FIFO (First In First Out)
**File:** `FIFO.cpp`

The simplest page replacement algorithm that removes the oldest page in memory.

- **Time Complexity:** O(1) for each page access
- **Space Complexity:** O(capacity)
- **Characteristics:** 
  - Easy to implement
  - May suffer from Belady's anomaly
  - Not optimal in most cases

### 2. LRU (Least Recently Used) - Counter Implementation
**File:** `LRU_counter.cpp`

Removes the page that has been unused for the longest time using timestamps.

- **Time Complexity:** O(n) for each page access
- **Space Complexity:** O(capacity)
- **Characteristics:**
  - Good performance in practice
  - Requires timestamp tracking
  - No Belady's anomaly

### 3. LRU (Least Recently Used) - Stack Implementation
**File:** `LRU_stack.cpp`

Efficient LRU implementation using a doubly linked list and hash map.

- **Time Complexity:** O(1) for each page access
- **Space Complexity:** O(capacity)
- **Characteristics:**
  - Optimal LRU implementation
  - Constant time operations
  - More complex implementation

### 4. LFU (Least Frequently Used)
**File:** `LFU.cpp`

Removes the page with the lowest frequency of access.

- **Time Complexity:** O(n) for each page access
- **Space Complexity:** O(capacity)
- **Characteristics:**
  - Considers access frequency
  - Good for some access patterns
  - May keep old pages too long

### 5. MFU (Most Frequently Used)
**File:** `MFU.cpp`

Removes the page with the highest frequency of access (opposite of LFU).

- **Time Complexity:** O(n) for each page access
- **Space Complexity:** O(capacity)
- **Characteristics:**
  - Counterintuitive but useful in some scenarios
  - Assumes frequently used pages won't be needed soon
  - Rarely used in practice

### 6. Optimal (OPT)
**File:** `OPT.cpp`

Theoretical algorithm that removes the page that will be used furthest in the future.

- **Time Complexity:** O(n²) for each page access
- **Space Complexity:** O(capacity)
- **Characteristics:**
  - Optimal (minimum page faults)
  - Requires future knowledge
  - Used as benchmark for other algorithms

### 7. Reference Bit (Clock Algorithm)
**File:** `ReferrenceBit.cpp`

Approximates LRU using a single reference bit per page.

- **Time Complexity:** O(n) worst case, O(1) average
- **Space Complexity:** O(capacity)
- **Characteristics:**
  - Hardware support for reference bits
  - Good approximation of LRU
  - Circular buffer implementation

### 8. Second Chance
**File:** `secondChance.cpp`

FIFO with a second chance for recently referenced pages.

- **Time Complexity:** O(n) worst case
- **Space Complexity:** O(capacity)
- **Characteristics:**
  - Improvement over basic FIFO
  - Uses reference bits
  - Better than FIFO, worse than LRU

### 9. Additional Reference Bit
**File:** `additionalReferenceBit.cpp`

Uses multiple bits to track page reference history.

- **Time Complexity:** O(n) for each page access
- **Space Complexity:** O(capacity)
- **Characteristics:**
  - More precise than single reference bit
  - Configurable number of history bits
  - Better approximation of LRU

### 10. Advanced Second Chance
**File:** `advancedSecondChance.cpp`

Uses both reference and modify bits to classify pages into four categories.

- **Time Complexity:** O(n) for each page access
- **Space Complexity:** O(capacity)
- **Characteristics:**
  - Considers both reference and modify bits
  - Four-class priority system
  - Avoids writing clean pages when possible

## Compilation and Execution

To compile and run any algorithm:

```bash
g++ -o algorithm_name algorithm_file.cpp
./algorithm_name
```

For example:
```bash
g++ -o fifo FIFO.cpp
./fifo
```

## Test Data

All algorithms use the same test data for comparison:
- **Page Reference String:** `7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2`
- **Frame Capacity:** `3`

## Performance Comparison

Based on the test data, here are the typical page fault counts:

| Algorithm | Page Faults | Page Fault Rate |
|-----------|-------------|-----------------|
| Optimal | 7 | 53.8% |
| LRU Stack | 9 | 69.2% |
| LRU Counter | 9 | 69.2% |
| FIFO | 10 | 76.9% |
| Second Chance | 10 | 76.9% |
| Reference Bit | 10 | 76.9% |
| LFU | 10 | 76.9% |
| MFU | 12 | 92.3% |
| Additional Reference Bit | Varies | Depends on bits |
| Advanced Second Chance | Varies | Depends on modify pattern |

## Key Concepts

### Belady's Anomaly
Some algorithms (like FIFO) may experience more page faults when given more memory frames. This counterintuitive behavior is called Belady's anomaly.

### Locality of Reference
- **Temporal Locality:** Recently accessed pages are likely to be accessed again
- **Spatial Locality:** Pages near recently accessed pages are likely to be accessed

### Stack Algorithms
Algorithms like LRU and Optimal are "stack algorithms" - they don't suffer from Belady's anomaly.

## Implementation Notes

1. **Error Handling:** Basic implementations without extensive error checking
2. **Output Format:** Each algorithm provides detailed step-by-step execution trace
3. **Data Structures:** Various data structures used based on algorithm requirements
4. **Optimization:** Focus on clarity over micro-optimizations

## Educational Purpose

These implementations are designed for:
- Understanding algorithm mechanics
- Comparing algorithm performance
- Learning data structure applications
- Operating systems coursework

## Future Enhancements

Potential improvements:
- Graphical visualization
- Performance benchmarking suite
- More sophisticated test cases
- Memory usage analysis
- Multi-threading support

## References

- Operating System Concepts by Silberschatz, Galvin, and Gagne
- Modern Operating Systems by Andrew S. Tanenbaum
- Operating Systems: Three Easy Pieces by Remzi H. Arpaci-Dusseau