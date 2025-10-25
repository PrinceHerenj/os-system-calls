# Multi-Elevator Control System Architecture

## Overview

This document describes the architecture and implementation of a **multi-threaded, multi-process elevator simulation** using **POSIX IPC mechanisms** (shared memory, message queues, and semaphores) to coordinate between multiple elevators serving multiple floors.

## System Architecture

### High-Level Design

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Request        │    │  Elevator       │    │  Elevator       │
│  Generator      │    │  Thread 0       │    │  Thread 1       │
│  Thread         │    │                 │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
                ┌────────────────────────────────┐
                │        Shared Memory           │
                │     (SystemData struct)        │
                │                                │
                │  ┌─────────────────────────┐   │
                │  │  ElevatorData[0]        │   │
                │  │  ElevatorData[1]        │   │
                │  │  ...                    │   │
                │  └─────────────────────────┘   │
                └────────────────────────────────┘
                                 │
                ┌────────────────────────────────┐
                │       Message Queue            │
                │   (RequestMessage routing)     │
                └────────────────────────────────┘
                                 │
                ┌────────────────────────────────┐
                │        Semaphores              │
                │  - System Semaphore            │
                │  - Elevator Semaphores[N]      │
                └────────────────────────────────┘
```

## Key Data Structures

### 1. ElevatorState Enum
```cpp
typedef enum {
    IDLE,           // Elevator not moving, no requests
    MOVING_UP,      // Elevator ascending
    MOVING_DOWN,    // Elevator descending
    DOOR_OPEN       // Doors open for passenger exchange
} ElevatorState;
```

### 2. RequestMessage Structure
```cpp
typedef struct {
    long mtype;         // Message type for queue routing (elevator_id + 1)
    int from_floor;     // Pickup floor
    int to_floor;       // Destination floor
    int elevator_id;    // Assigned elevator ID
    int request_id;     // Unique request identifier
} RequestMessage;
```

**Purpose**: Communication medium between request generator and elevator threads via message queues.

### 3. PendingRequest Structure
```cpp
typedef struct {
    int from_floor;     // Pickup floor
    int to_floor;       // Destination floor
    int request_id;     // Request identifier
} PendingRequest;
```

**Purpose**: Temporary storage for requests until passengers actually board the elevator.

### 4. ElevatorData Structure
```cpp
typedef struct {
    int current_floor;                      // Current elevator position
    ElevatorState state;                    // Current operational state
    int passengers;                         // Current passenger count
    bool pickup_requests[MAX_FLOORS];       // Floors with waiting passengers
    bool dropoff_requests[MAX_FLOORS];      // Floors where passengers exit
    int direction;                          // Movement direction (1=up, -1=down, 0=idle)
    PendingRequest pending_requests[20];    // Unprocessed requests
    int pending_count;                      // Number of pending requests
} ElevatorData;
```

**Key Innovation**: Separation of pickup vs. dropoff requests enables realistic passenger flow simulation.

### 5. SystemData Structure
```cpp
typedef struct {
    ElevatorData elevators[MAX_ELEVATORS];  // Array of all elevators
    int num_elevators;                      // System elevator count
    int num_floors;                         // System floor count
    int total_requests;                     // Global request counter
} SystemData;
```

## IPC Mechanisms

### 1. Shared Memory
- **Purpose**: Central storage for all system state
- **Scope**: All elevator data accessible by all threads
- **Benefits**: Real-time coordination, efficient data sharing
- **Implementation**: Single shared memory segment containing `SystemData`

### 2. Message Queues
- **Purpose**: Communication channel for request distribution
- **Routing**: Each elevator has unique message type (`elevator_id + 1`)
- **Mode**: Non-blocking retrieval (`IPC_NOWAIT`) prevents thread blocking
- **Benefits**: Asynchronous request handling, load distribution

### 3. Semaphores
- **System Semaphore**: Protects global system data (request counts)
- **Elevator Semaphores**: One per elevator for individual data protection
- **Benefits**: Prevents race conditions, ensures data consistency

## Core Algorithms

### 1. Elevator Selection Algorithm

**Objective**: Assign incoming requests to the most appropriate elevator.

**Selection Criteria** (in priority order):
1. **Idle elevators**: Highest priority (-5 distance penalty)
2. **Direction alignment**: Elevators moving toward pickup floor (-3 penalty)
3. **Physical proximity**: Closest elevator by floor distance

```cpp
int find_best_elevator(int from_floor, int to_floor) {
    for each elevator {
        distance = abs(elevator_floor - from_floor);
        
        if (elevator.state == IDLE)
            distance -= 5;  // Strong preference for idle
            
        if (elevator moving in correct direction)
            distance -= 3;  // Prefer aligned movement
            
        if (distance < min_distance) {
            best_elevator = current_elevator;
            min_distance = distance;
        }
    }
    return best_elevator;
}
```

### 2. SCAN-like Movement Algorithm

**Objective**: Minimize elevator movement while serving all requests efficiently.

**Strategy**:
1. **Continue current direction** until no more requests ahead
2. **Reverse direction** only when necessary
3. **Batch requests** efficiently in the same direction

```cpp
int find_next_stop(ElevatorData *elevator) {
    // Priority 1: Current floor if pickup/dropoff needed
    if (requests_at_current_floor)
        return current_floor;
    
    // Priority 2: Continue in current direction
    if (elevator->direction == UP)
        return next_floor_above_with_requests;
    else if (elevator->direction == DOWN)
        return next_floor_below_with_requests;
    
    // Priority 3: Reverse direction if no requests ahead
    return next_floor_in_opposite_direction;
}
```

### 3. Two-Phase Request Processing

**Objective**: Realistic passenger boarding simulation.

**Phase 1 - Request Assignment**:
- Mark pickup floor when request arrives
- Add request to pending list
- Elevator begins moving toward pickup floor

**Phase 2 - Passenger Boarding**:
- When elevator reaches pickup floor, passengers board
- Dropoff destinations are marked
- Pending requests are removed from queue

```cpp
// Phase 1: Request received
elevator->pickup_requests[from_floor] = true;
add_to_pending_requests(request);

// Phase 2: Elevator arrives at pickup floor
for each pending_request at current_floor {
    elevator->dropoff_requests[to_floor] = true;
    elevator->passengers++;
    remove_from_pending_requests(request);
}
```

## Thread Architecture

### Main Thread
**Responsibilities**:
- System initialization (IPC setup)
- Thread creation and management
- Cleanup and resource deallocation

### Request Generator Thread
**Responsibilities**:
- Generate random passenger requests
- Select optimal elevator for each request
- Send requests via message queue
- Simulate realistic request timing

**Workflow**:
```
for each request {
    generate_random_floors();
    elevator_id = find_best_elevator();
    send_message_to_elevator(elevator_id);
    sleep(random_interval);
}
```

### Elevator Threads (One per Elevator)
**Responsibilities**:
- Process incoming requests (non-blocking)
- Execute elevator movement and stops
- Handle passenger boarding/exit
- Maintain elevator state

**Workflow**:
```
while (true) {
    check_for_new_requests();
    process_elevator_movement();
    handle_door_operations();
    sleep(1_second);
}
```

## Synchronization Strategy

### Critical Section Protection
1. **System-wide data**: Protected by `system_semid`
2. **Individual elevator data**: Protected by `elevator_semid[i]`
3. **Elevator selection**: Uses both semaphores for consistency

### Deadlock Prevention
- **Consistent lock ordering**: System semaphore → Elevator semaphores
- **Short critical sections**: Minimize lock holding time
- **Non-blocking operations**: Message queue operations use `IPC_NOWAIT`

### Race Condition Prevention
- **Atomic operations**: Semaphore-protected state changes
- **Message ordering**: Queue-based request distribution
- **State consistency**: Coordinated elevator data updates

## Performance Characteristics

### Efficiency Optimizations
1. **SCAN Algorithm**: Reduces unnecessary elevator movements
2. **Smart Dispatching**: Minimizes total system travel time
3. **Load Balancing**: Distributes requests across available elevators
4. **Batching**: Groups requests in same direction

### Scalability Features
- **Configurable Parameters**: Adjustable elevator/floor counts
- **Modular Design**: Easy to add new elevator algorithms
- **IPC-based**: Supports distributed system deployment
- **Thread-safe**: Concurrent request processing

### Resource Management
- **Memory Efficiency**: Shared memory for system state
- **CPU Utilization**: Non-blocking operations prevent thread starvation
- **I/O Optimization**: Asynchronous message passing

## Usage Examples

### Basic Execution
```bash
# 2 elevators, 10 floors, 5 requests
./elevatorBuild 2 10 5
```

### Sample Output Analysis
```
>> NEW REQUEST 0: Floor 1 -> Floor 4 (assigned to Elevator 0)
Elevator 0: Moving UP to floor 1 (target: 1)
Elevator 0: DOOR OPEN at floor 1 (PICKUP)
    Passenger 0 boarded, going to floor 4
Elevator 0: Moving UP to floor 4 (target: 4)
Elevator 0: DOOR OPEN at floor 4 (DROPOFF)
```

### Performance Metrics
- **Request Completion Time**: Time from request to passenger dropoff
- **Elevator Utilization**: Percentage of time elevators are active
- **Average Wait Time**: Time passengers wait for pickup
- **System Throughput**: Requests processed per unit time

## System Limitations and Constraints

### Current Limitations
- **Maximum Elevators**: 5 (configurable via `MAX_ELEVATORS`)
- **Maximum Floors**: 10 (configurable via `MAX_FLOORS`)
- **Pending Requests**: 20 per elevator
- **Single Building**: No multi-building support

### Potential Enhancements
1. **Dynamic Load Balancing**: Real-time algorithm adjustment
2. **Predictive Scheduling**: ML-based request prediction
3. **Energy Optimization**: Power-aware movement algorithms
4. **Maintenance Mode**: Support for elevator servicing
5. **Priority Requests**: VIP or emergency request handling

## Conclusion

This elevator control system demonstrates a **production-quality architecture** with:
- **Realistic passenger simulation** through two-phase request processing
- **Efficient algorithms** minimizing wait times and energy consumption
- **Robust synchronization** ensuring thread safety and data consistency
- **Scalable design** supporting multiple elevators and configurable parameters
- **Comprehensive IPC usage** showcasing shared memory, message queues, and semaphores

The implementation serves as an excellent example of **systems programming**, **concurrent algorithms**, and **real-time coordination** in complex multi-threaded applications.