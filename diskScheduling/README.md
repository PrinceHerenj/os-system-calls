# Disk Scheduling Algorithms

This directory contains implementations of four fundamental disk scheduling algorithms used in operating systems for optimizing disk access patterns and reducing seek time.

## Algorithms Implemented

1. **FCFS** - First Come First Serve
2. **SSTF** - Shortest Seek Time First
3. **SCAN** - Elevator Algorithm
4. **C-SCAN** - Circular SCAN

## Overview

Disk scheduling algorithms determine the order in which disk I/O requests are serviced. The primary goal is to minimize the total seek time, which is the time required to move the disk head from one track to another.

## Common Test Data

All algorithms use the same test scenario for fair comparison:

**System Configuration:**
- **Disk Size:** 0 to 199 cylinders (200 total)
- **Initial Head Position:** 53
- **Request Queue:** [98, 183, 37, 122, 14, 124, 65, 67]
- **Total Requests:** 8

**Request Distribution:**
- Requests < Head (53): [37, 14]
- Requests ≥ Head (53): [98, 183, 122, 124, 65, 67]

## Algorithm Results Comparison

### Performance Summary

| Algorithm | Total Seek Time | Average Seek Time | Seek Sequence |
|-----------|-----------------|-------------------|---------------|
| FCFS      | 640            | 80.00            | 53→98→183→37→122→14→124→65→67 |
| SSTF      | 236            | 29.50            | 53→65→67→37→14→98→122→124→183 |
| SCAN      | 208            | 26.00            | 53→65→67→98→122→124→183→199→37→14 |
| C-SCAN    | 382            | 47.75            | 53→65→67→98→122→124→183→199→0→14→37 |

## Detailed Algorithm Analysis

### 1. FCFS (First Come First Serve)

**Execution Order:** Processes requests in arrival order
```
53 → 98 → 183 → 37 → 122 → 14 → 124 → 65 → 67
```

**Seek Pattern:**
- 53 to 98: 45 cylinders
- 98 to 183: 85 cylinders
- 183 to 37: 146 cylinders (longest seek)
- 37 to 122: 85 cylinders
- 122 to 14: 108 cylinders
- 14 to 124: 110 cylinders
- 124 to 65: 59 cylinders
- 65 to 67: 2 cylinders

**Characteristics:**
- ✅ Simple and fair
- ❌ Poor performance (highest seek time)
- ❌ No optimization
- ❌ Wild head movements

### 2. SSTF (Shortest Seek Time First)

**Execution Order:** Always chooses closest request
```
53 → 65 → 67 → 37 → 14 → 98 → 122 → 124 → 183
```

**Seek Pattern:**
- 53 to 65: 12 cylinders (closest to 53)
- 65 to 67: 2 cylinders (closest to 65)
- 67 to 37: 30 cylinders (closest to 67)
- 37 to 14: 23 cylinders (closest to 37)
- 14 to 98: 84 cylinders (closest to 14)
- 98 to 122: 24 cylinders (closest to 98)
- 122 to 124: 2 cylinders (closest to 122)
- 124 to 183: 59 cylinders (closest to 124)

**Characteristics:**
- ✅ Much better than FCFS
- ✅ Locally optimal choices
- ⚠️ May cause starvation
- ❌ Not globally optimal

### 3. SCAN (Elevator Algorithm)

**Execution Order:** Scans in one direction, then reverses
```
53 → 65 → 67 → 98 → 122 → 124 → 183 → 199 → 37 → 14
```

**Seek Pattern:**
- 53 to 65: 12 cylinders (right direction)
- 65 to 67: 2 cylinders
- 67 to 98: 31 cylinders
- 98 to 122: 24 cylinders
- 122 to 124: 2 cylinders
- 124 to 183: 59 cylinders
- 183 to 199: 16 cylinders (to disk end)
- 199 to 37: 162 cylinders (reverse direction)
- 37 to 14: 23 cylinders

**Characteristics:**
- ✅ No starvation
- ✅ Uniform wait times
- ✅ Better than SSTF overall
- ⚠️ Extra movement to disk ends

### 4. C-SCAN (Circular SCAN)

**Execution Order:** Scans in one direction, jumps to other end
```
53 → 65 → 67 → 98 → 122 → 124 → 183 → 199 → 0 → 14 → 37
```

**Seek Pattern:**
- 53 to 65: 12 cylinders
- 65 to 67: 2 cylinders
- 67 to 98: 31 cylinders
- 98 to 122: 24 cylinders
- 122 to 124: 2 cylinders
- 124 to 183: 59 cylinders
- 183 to 199: 16 cylinders (to end)
- 199 to 0: 199 cylinders (circular jump)
- 0 to 14: 14 cylinders
- 14 to 37: 23 cylinders

**Characteristics:**
- ✅ More uniform wait times than SCAN
- ✅ Better for heavy loads
- ❌ Higher seek time due to circular jumps
- ⚠️ Treats disk as circular queue

## Visual Representation

### FCFS Movement Pattern
```
Position: 0    53   98   183   37   122   14   124   65   67   199
          |----*----*----*-----*----*-----*----*----*----*----|
               ↑    →    →     ←    →     ←    →    ←    →
```

### SSTF Movement Pattern
```
Position: 0   14   37   53   65   67   98   122  124  183   199
          |----*----*----*----*----*----*----*----*----*----|
                    ↑    →    →    →    →    →    →
```

### SCAN Movement Pattern
```
Position: 0   14   37   53   65   67   98   122  124  183   199
          |----*----*----*----*----*----*----*----*----*----|
                    ↑    →    →    →    →    →    →    ←    ←
```

### C-SCAN Movement Pattern
```
Position: 0   14   37   53   65   67   98   122  124  183   199
          |----*----*----*----*----*----*----*----*----*----|
               ↑         ↑    →    →    →    →    →    →    ↻
```

## Performance Analysis

### Seek Time Distribution

| Algorithm | Min Seek | Max Seek | Variance | Efficiency |
|-----------|----------|----------|----------|------------|
| FCFS      | 2        | 146      | High     | Poor       |
| SSTF      | 2        | 84       | Medium   | Good       |
| SCAN      | 2        | 162      | Low      | Better     |
| C-SCAN    | 2        | 199      | Medium   | Good       |

### Use Case Recommendations

**FCFS:**
- Simple batch systems
- Single-user environments
- When fairness is priority over performance

**SSTF:**
- Interactive systems with moderate load
- When response time is critical
- Systems with predictable access patterns

**SCAN:**
- Multi-user systems
- Heavy I/O workloads
- When starvation prevention is important
- Real-time systems

**C-SCAN:**
- Systems with uniform access patterns
- Heavy concurrent access
- When consistent response time is crucial
- High-performance servers

## Key Concepts

### Seek Time Components
1. **Seek Time:** Time to move head to correct track
2. **Rotational Delay:** Time for sector to rotate under head
3. **Transfer Time:** Time to read/write data

### Algorithm Selection Criteria
1. **Throughput:** Requests processed per unit time
2. **Response Time:** Time from request to completion
3. **Fairness:** Equal treatment of all requests
4. **Starvation:** Whether requests can wait indefinitely

### Trade-offs

**Performance vs Fairness:**
- SSTF: High performance, potential starvation
- FCFS: Fair but poor performance
- SCAN/C-SCAN: Balanced approach

**Simplicity vs Optimization:**
- FCFS: Simple but unoptimized
- SSTF: More complex, locally optimal
- SCAN: Complex but globally better

## Compilation and Execution

```bash
# Compile individual algorithms
g++ -o fcfs FCFS.cpp
g++ -o sstf SSTF.cpp
g++ -o scan SCAN.cpp
g++ -o cscan C-SCAN.cpp

# Run algorithms
./fcfs
./sstf
./scan
./cscan

# Compile all at once
g++ -o fcfs FCFS.cpp && g++ -o sstf SSTF.cpp && g++ -o scan SCAN.cpp && g++ -o cscan C-SCAN.cpp
```

## Real-World Applications

### Modern Storage Systems
- **SSDs:** Use different algorithms due to no mechanical movement
- **HDDs:** Still benefit from these classical algorithms
- **RAID Systems:** Apply scheduling at multiple levels

### Operating System Integration
- **Linux:** Uses deadline, CFQ, and mq-deadline schedulers
- **Windows:** Uses SCAN-based algorithms
- **Real-time OS:** Often use SCAN for predictability

## Algorithm Evolution

**Historical Development:**
1. **FCFS (1960s):** First simple approach
2. **SSTF (1970s):** Performance optimization
3. **SCAN (1970s):** Fairness consideration
4. **C-SCAN (1980s):** Uniform response times

**Modern Adaptations:**
- **LOOK:** SCAN without going to disk ends
- **C-LOOK:** C-SCAN without going to disk ends
- **Anticipatory:** Predicts future requests
- **Deadline:** Ensures request deadlines are met

## Limitations and Considerations

### Classical Algorithm Limitations
1. **SSD Compatibility:** No mechanical movement in SSDs
2. **Request Prediction:** Don't consider future requests
3. **Priority Handling:** No support for request priorities
4. **Deadline Management:** No real-time guarantees

### Modern Enhancements
1. **I/O Merging:** Combine adjacent requests
2. **Request Reordering:** Within fairness constraints
3. **Priority Queues:** Different classes of requests
4. **Adaptive Algorithms:** Learn from access patterns

## Educational Value

These implementations demonstrate:
- **Algorithm Design:** Different optimization strategies
- **Trade-off Analysis:** Performance vs fairness
- **Data Structure Usage:** Various approaches to problem-solving
- **Performance Measurement:** Quantitative algorithm comparison

## Future Enhancements

Potential improvements:
- **Dynamic Visualization:** Real-time head movement display
- **Multiple Test Cases:** Various request patterns
- **Performance Benchmarking:** Statistical analysis
- **Hybrid Algorithms:** Combining multiple approaches
- **Real-time Constraints:** Deadline-aware scheduling

## References

- Operating System Concepts by Silberschatz, Galvin, and Gagne
- Modern Operating Systems by Andrew S. Tanenbaum
- Operating Systems: Three Easy Pieces by Remzi H. Arpaci-Dusseau
- Computer Systems: A Programmer's Perspective by Bryant & O'Hallaron