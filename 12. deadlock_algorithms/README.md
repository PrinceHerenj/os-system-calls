# Deadlock Algorithms

This directory contains implementations of three fundamental deadlock-related algorithms used in operating systems for resource management and deadlock handling.

## Algorithms Implemented

1. **Banker's Algorithm** - Deadlock avoidance
2. **Deadlock Detection Algorithm** - Deadlock detection
3. **Resource Allocation Algorithm** - Safe resource allocation with banker's algorithm integration

## Overview

Deadlock is a situation where a set of processes are blocked because each process is holding a resource and waiting for another resource acquired by some other process. These algorithms help in preventing, avoiding, and detecting deadlocks in multi-process systems.

## Algorithm Details

### 1. Banker's Algorithm (`bankersAlgorithm.cpp`)

**Purpose:** Deadlock avoidance using safety algorithm

**Key Features:**
- Determines if the system is in a safe state
- Finds a safe sequence of process execution
- Prevents deadlock by ensuring safe resource allocation

**Algorithm Steps:**
1. Initialize Work = Available
2. Find process i where Finish[i] = false and Need[i] ≤ Work
3. Set Work = Work + Allocation[i] and Finish[i] = true
4. Repeat until all processes finish or no such process found
5. If all processes finish, system is in safe state

**Test Data:**
- 5 processes (P0-P4)
- 3 resource types (R0-R2)
- Available resources: [3, 3, 2]

### 2. Deadlock Detection Algorithm (`detectionAlgorithms.cpp`)

**Purpose:** Detect if deadlock currently exists in the system

**Key Features:**
- Comprehensive deadlock detection with multiple test cases
- Step-by-step execution visualization
- Identifies deadlocked processes
- Three different test scenarios

**Algorithm Steps:**
1. Initialize Work = Available
2. Initialize Finish[i] = false if process holds resources, true otherwise
3. Find process i where Finish[i] = false and Request[i] ≤ Work
4. Set Work = Work + Allocation[i] and Finish[i] = true
5. Repeat until no such process found
6. If any Finish[i] = false, deadlock exists

**Test Cases:**
- **Test Case 1:** No deadlock scenario
- **Test Case 2:** Deadlock present scenario
- **Test Case 3:** Mixed scenario with available resources

### 3. Resource Allocation Algorithm (`resourceAllocationAlgorithm.cpp`)

**Purpose:** Safe resource allocation using banker's algorithm principles

**Key Features:**
- Dynamic resource request handling
- Safety verification before allocation
- Resource release functionality
- Complete simulation with multiple requests

**Algorithm Steps:**
1. Check if Request[i] ≤ Need[i]
2. Check if Request[i] ≤ Available
3. Temporarily allocate resources:
   - Available = Available - Request[i]
   - Allocation[i] = Allocation[i] + Request[i]
   - Need[i] = Need[i] - Request[i]
4. Run safety algorithm to verify safe state
5. If safe, grant request; otherwise, restore state and deny

## Common Test Data

All algorithms use consistent data for comparison:

### System Configuration
- **Processes:** 5 (P0, P1, P2, P3, P4)
- **Resource Types:** 3 (R0, R1, R2)
- **Total Resources:** [10, 5, 7]

### Maximum Demand Matrix
| Process | R0 | R1 | R2 |
|---------|----|----|----| 
| P0      | 7  | 5  | 3  |
| P1      | 3  | 2  | 2  |
| P2      | 9  | 0  | 2  |
| P3      | 2  | 2  | 2  |
| P4      | 4  | 3  | 3  |

### Initial Allocation Matrix
| Process | R0 | R1 | R2 |
|---------|----|----|----| 
| P0      | 0  | 1  | 0  |
| P1      | 2  | 0  | 0  |
| P2      | 3  | 0  | 2  |
| P3      | 2  | 1  | 1  |
| P4      | 0  | 0  | 2  |

### Derived Need Matrix
| Process | R0 | R1 | R2 |
|---------|----|----|----| 
| P0      | 7  | 4  | 3  |
| P1      | 1  | 2  | 2  |
| P2      | 6  | 0  | 0  |
| P3      | 0  | 1  | 1  |
| P4      | 4  | 3  | 1  |

### Available Resources
- **Initial Available:** [3, 3, 2]

## Compilation and Execution

To compile and run each algorithm:

```bash
# Banker's Algorithm
g++ -o bankers bankersAlgorithm.cpp
./bankers

# Deadlock Detection
g++ -o detection detectionAlgorithms.cpp
./detection

# Resource Allocation
g++ -o allocation resourceAllocationAlgorithm.cpp
./allocation
```

## Expected Results

### Banker's Algorithm Output
```
System is in safe state
Safe sequence: P1 -> P3 -> P0 -> P2 -> P4
```

### Deadlock Detection Results
- **Test Case 1:** No deadlock - Safe sequence: P0 -> P2 -> P1 -> P3 -> P4
- **Test Case 2:** Deadlock detected - All processes deadlocked
- **Test Case 3:** No deadlock - Safe sequence: P0 -> P1 -> P2 -> P3 -> P4

### Resource Allocation Simulation
- Multiple request scenarios with safety verification
- Resource release and reallocation
- Dynamic state management

## Key Concepts

### Safe State
A state where there exists a safe sequence of process execution that allows all processes to complete without deadlock.

### Safe Sequence
An ordering of processes such that each process can complete with available resources plus resources released by previously completed processes.

### Need Matrix
Calculated as: Need[i][j] = Max[i][j] - Allocation[i][j]

### Deadlock Conditions
1. **Mutual Exclusion:** Resources cannot be shared
2. **Hold and Wait:** Processes hold resources while waiting for others
3. **No Preemption:** Resources cannot be forcibly taken
4. **Circular Wait:** Circular dependency of resource requests

## Algorithm Comparison

| Feature | Banker's | Detection | Resource Allocation |
|---------|----------|-----------|-------------------|
| Purpose | Avoidance | Detection | Dynamic Allocation |
| When Used | Before allocation | Periodically | On resource request |
| Complexity | O(m×n²) | O(m×n²) | O(m×n²) |
| Prevention | Yes | No | Yes |
| Detection | No | Yes | No |

## Real-World Applications

- **Operating Systems:** Process and resource management
- **Database Systems:** Transaction deadlock detection
- **Distributed Systems:** Resource allocation in clusters
- **Real-time Systems:** Critical resource management

## Educational Notes

These implementations demonstrate:
- Systematic approach to deadlock handling
- Step-by-step algorithm execution
- Multiple test scenarios
- Clear visualization of algorithm states
- Practical resource management concepts

## Limitations and Considerations

1. **Assumption:** Fixed number of resources and processes
2. **Overhead:** Algorithms have computational complexity
3. **Practicality:** Real systems may use simplified approaches
4. **Starvation:** Some processes may wait indefinitely

## Future Enhancements

Potential improvements:
- Dynamic process creation/termination
- Priority-based resource allocation
- Distributed deadlock detection
- Performance optimization
- Graphical visualization
- Integration with actual OS scheduling

## References

- Operating System Concepts by Silberschatz, Galvin, and Gagne
- Modern Operating Systems by Andrew S. Tanenbaum
- Operating Systems: Internals and Design Principles by William Stallings