# CPU Scheduling Algorithms

This directory contains implementations of five fundamental CPU scheduling algorithms used in operating systems. Each algorithm demonstrates different approaches to process scheduling with their unique characteristics and trade-offs.

## Algorithms Implemented

1. **FCFS** - First Come First Serve
2. **SJF** - Shortest Job First
3. **SRTF** - Shortest Remaining Time First
4. **RR** - Round Robin
5. **Priority** - Priority Scheduling

## Common Test Data

All algorithms use the same set of 5 processes for comparison:

| Process | Arrival Time | Burst Time | Priority |
|---------|--------------|------------|----------|
| P1      | 0            | 10         | 3        |
| P2      | 1            | 4          | 1        |
| P3      | 2            | 6          | 4        |
| P4      | 3            | 8          | 2        |
| P5      | 4            | 3          | 5        |

**Note:** For Round Robin, Time Quantum = 2

## Algorithm Results Comparison

### Performance Metrics

| Algorithm | Avg TAT | Avg WT | Avg RT | Total Time |
|-----------|---------|--------|--------|------------|
| FCFS      | 17.40   | 8.00   | 8.00   | 31         |
| SJF       | 12.80   | 5.60   | 5.60   | 31         |
| SRTF      | 8.60    | 3.00   | 2.40   | 31         |
| RR (TQ=2) | 16.40   | 9.20   | 4.00   | 31         |
| Priority  | 13.00   | 5.80   | 5.80   | 31         |

**Legend:**
- TAT = Turnaround Time
- WT = Waiting Time  
- RT = Response Time

## Detailed Execution Analysis

### 1. FCFS (First Come First Serve)

**Execution Order:** P1 → P2 → P3 → P4 → P5

| Process | AT | BT | CT | TAT | WT | RT |
|---------|----|----|----|----|----|----|
| P1      | 0  | 10 | 10 | 10  | 0  | 0  |
| P2      | 1  | 4  | 14 | 13  | 9  | 9  |
| P3      | 2  | 6  | 20 | 18  | 12 | 12 |
| P4      | 3  | 8  | 28 | 25  | 17 | 17 |
| P5      | 4  | 3  | 31 | 27  | 24 | 24 |

**Gantt Chart:**
```
| P1 | P2 | P3 | P4 | P5 |
0   10   14   20   28   31
```

### 2. SJF (Shortest Job First)

**Execution Order:** P1 → P5 → P2 → P3 → P4

| Process | AT | BT | CT | TAT | WT | RT |
|---------|----|----|----|----|----|----|
| P1      | 0  | 10 | 10 | 10  | 0  | 0  |
| P2      | 1  | 4  | 17 | 16  | 12 | 12 |
| P3      | 2  | 6  | 23 | 21  | 15 | 15 |
| P4      | 3  | 8  | 31 | 28  | 20 | 20 |
| P5      | 4  | 3  | 13 | 9   | 6  | 6  |

**Gantt Chart:**
```
| P1 | P5 | P2 | P3 | P4 |
0   10   13   17   23   31
```

### 3. SRTF (Shortest Remaining Time First)

**Execution Order (with preemption):** P1 → P2 → P5 → P1 → P3 → P4

| Process | AT | BT | CT | TAT | WT | RT |
|---------|----|----|----|----|----|----|
| P1      | 0  | 10 | 23 | 23  | 13 | 0  |
| P2      | 1  | 4  | 5  | 4   | 0  | 0  |
| P3      | 2  | 6  | 29 | 27  | 21 | 21 |
| P4      | 3  | 8  | 37 | 34  | 26 | 26 |
| P5      | 4  | 3  | 8  | 4   | 1  | 1  |

**Gantt Chart:**
```
| P1 | P2 | P5 | P1 | P3 | P4 |
0    1    5    8   17   23   31
```

### 4. Round Robin (Time Quantum = 2)

**Execution with Time Slicing:**

| Process | AT | BT | CT | TAT | WT | RT |
|---------|----|----|----|----|----|----|
| P1      | 0  | 10 | 30 | 30  | 20 | 0  |
| P2      | 1  | 4  | 11 | 10  | 6  | 1  |
| P3      | 2  | 6  | 21 | 19  | 13 | 4  |
| P4      | 3  | 8  | 31 | 28  | 20 | 7  |
| P5      | 4  | 3  | 16 | 12  | 9  | 9  |

**Gantt Chart:**
```
| P1 | P2 | P1 | P3 | P2 | P4 | P1 | P5 | P3 | P4 | P1 | P3 | P4 | P1 | P4 |
0    2    4    6    8   10   12   14   16   18   20   22   24   26   28   30
```

### 5. Priority Scheduling

**Execution Order (by priority):** P1 → P2 → P4 → P3 → P5

| Process | AT | BT | Priority | CT | TAT | WT | RT |
|---------|----|----|----------|----|----|----|----|
| P1      | 0  | 10 | 3        | 10 | 10  | 0  | 0  |
| P2      | 1  | 4  | 1        | 14 | 13  | 9  | 9  |
| P3      | 2  | 6  | 4        | 28 | 26  | 20 | 20 |
| P4      | 3  | 8  | 2        | 22 | 19  | 11 | 11 |
| P5      | 4  | 3  | 5        | 31 | 27  | 24 | 24 |

**Gantt Chart:**
```
| P1 | P2 | P4 | P3 | P5 |
0   10   14   22   28   31
```

## Algorithm Characteristics

### FCFS (First Come First Serve)
- **Type:** Non-preemptive
- **Criteria:** Arrival time
- **Advantages:** Simple to implement, no starvation
- **Disadvantages:** Poor average waiting time, convoy effect
- **Best for:** Batch systems

### SJF (Shortest Job First)
- **Type:** Non-preemptive
- **Criteria:** Burst time
- **Advantages:** Optimal average waiting time
- **Disadvantages:** Starvation of long processes, requires burst time knowledge
- **Best for:** Systems with known execution times

### SRTF (Shortest Remaining Time First)
- **Type:** Preemptive
- **Criteria:** Remaining time
- **Advantages:** Better response time than SJF
- **Disadvantages:** High context switching overhead, starvation possible
- **Best for:** Interactive systems

### Round Robin
- **Type:** Preemptive
- **Criteria:** Time quantum
- **Advantages:** Fair CPU allocation, good response time
- **Disadvantages:** Higher average turnaround time, context switching overhead
- **Best for:** Time-sharing systems

### Priority Scheduling
- **Type:** Non-preemptive (this implementation)
- **Criteria:** Process priority
- **Advantages:** Important processes get preference
- **Disadvantages:** Starvation of low priority processes
- **Best for:** Real-time systems

## Key Observations

1. **Best Average Waiting Time:** SRTF (3.00) < SJF (5.60) < Priority (5.80)
2. **Best Response Time:** SRTF (2.40) < RR (4.00) < SJF/Priority (5.60/5.80)
3. **Fairness:** Round Robin provides most balanced CPU allocation
4. **Starvation Risk:** SJF, SRTF, and Priority can cause starvation
5. **Context Switching:** SRTF and RR have highest overhead

## Compilation and Execution

To compile and run any algorithm:

```bash
g++ -o algorithm_name algorithm_file.cpp
./algorithm_name
```

Examples:
```bash
g++ -o fcfs FCFS.cpp && ./fcfs
g++ -o sjf SJF.cpp && ./sjf
g++ -o srtf SRTF.cpp && ./srtf
g++ -o rr RR.cpp && ./rr
g++ -o priority Priority.cpp && ./priority
```

## Understanding the Metrics

### Turnaround Time (TAT)
- **Formula:** Completion Time - Arrival Time
- **Meaning:** Total time a process spends in the system

### Waiting Time (WT)
- **Formula:** Turnaround Time - Burst Time
- **Meaning:** Time a process waits in the ready queue

### Response Time (RT)
- **Formula:** First CPU Time - Arrival Time
- **Meaning:** Time from arrival until first CPU allocation

### Completion Time (CT)
- **Meaning:** Time when process finishes execution

## Real-World Applications

- **FCFS:** Simple batch processing systems
- **SJF:** Job scheduling in batch systems with known execution times
- **SRTF:** Interactive systems requiring quick response
- **Round Robin:** Time-sharing operating systems (Unix, Windows)
- **Priority:** Real-time systems, system processes vs user processes

## Educational Notes

This implementation focuses on:
- Clear algorithm demonstration
- Step-by-step execution visualization
- Performance metric calculation
- Comparative analysis

For production systems, additional considerations include:
- Aging mechanisms to prevent starvation
- Multi-level queues
- Dynamic priority adjustment
- I/O scheduling integration