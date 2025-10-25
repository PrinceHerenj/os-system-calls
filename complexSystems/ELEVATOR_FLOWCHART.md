# Elevator Control System Flowchart

## Main Program Flow

```mermaid
flowchart TD
    A[Start Program] --> B[Parse Command Line Arguments]
    B --> C{Valid Parameters?}
    C -->|No| D[Print Error & Exit]
    C -->|Yes| E[Initialize Random Seed]
    E --> F[Initialize System]
    F --> G[Create Elevator Threads]
    G --> H[Create Request Generator Thread]
    H --> I[Wait for Request Generator to Finish]
    I --> J[Wait 20 seconds for Elevators]
    J --> K[Cancel Elevator Threads]
    K --> L[Cleanup System]
    L --> M[End Program]
```

## System Initialization Flow

```mermaid
flowchart TD
    A[initialize_system] --> B[Generate IPC Keys using ftok]
    B --> C[Create Shared Memory Segment]
    C --> D{Shared Memory Created?}
    D -->|No| E[Print Error & Exit]
    D -->|Yes| F[Attach to Shared Memory]
    F --> G[Create Message Queue]
    G --> H{Message Queue Created?}
    H -->|No| I[Print Error & Exit]
    H -->|Yes| J[Create System Semaphore]
    J --> K[Create Elevator Semaphores Array]
    K --> L[Initialize System Data Structure]
    L --> M[Initialize All Elevators to Floor 0, IDLE State]
    M --> N[Clear All Request Arrays]
    N --> O[System Initialization Complete]
```

## Request Generator Thread Flow

```mermaid
flowchart TD
    A[request_generator Thread Start] --> B[Print Start Message]
    B --> C[Sleep 2 seconds]
    C --> D[Initialize Request Counter i=0]
    D --> E{i < num_requests?}
    E -->|No| F[Print Completion Message & Exit]
    E -->|Yes| G[Generate Random from_floor]
    G --> H[Generate Random to_floor]
    H --> I{from_floor == to_floor?}
    I -->|Yes| H
    I -->|No| J[Call find_best_elevator]
    J --> K[Set Message Type = elevator_id + 1]
    K --> L[Send Message to Selected Elevator]
    L --> M{Message Sent Successfully?}
    M -->|No| N[Print Error]
    M -->|Yes| O[Acquire System Semaphore]
    O --> P[Increment total_requests]
    P --> Q[Release System Semaphore]
    Q --> R[Sleep 2-4 seconds randomly]
    R --> S[Increment i]
    S --> E
```

## Elevator Selection Algorithm Flow

```mermaid
flowchart TD
    A[find_best_elevator] --> B[Initialize best=0, min_distance=floors+1]
    B --> C[Acquire System Semaphore]
    C --> D[Initialize loop i=0]
    D --> E{i < num_elevators?}
    E -->|No| F[Release System Semaphore]
    F --> G[Return best elevator]
    E -->|Yes| H[Acquire Elevator i Semaphore]
    H --> I[Calculate distance = abs(current_floor - from_floor)]
    I --> J{Elevator State == IDLE?}
    J -->|Yes| K[distance -= 5]
    J -->|No| L[Calculate Required Direction]
    K --> L
    L --> M{Elevator Moving in Right Direction?}
    M -->|Yes| N[distance -= 3]
    M -->|No| O{distance < min_distance?}
    N --> O
    O -->|Yes| P[best = i, min_distance = distance]
    O -->|No| Q[Release Elevator i Semaphore]
    P --> Q
    Q --> R[Increment i]
    R --> E
```

## Elevator Thread Flow

```mermaid
flowchart TD
    A[elevator_thread Start] --> B[Print Thread Started Message]
    B --> C[Enter Infinite Loop]
    C --> D[Check for New Messages - Non-blocking]
    D --> E{Message Received?}
    E -->|Yes| F[Print Request Received]
    F --> G[Acquire Elevator Semaphore]
    G --> H[Mark pickup_requests[from_floor] = true]
    H --> I{Pending Count < 20?}
    I -->|Yes| J[Add Request to pending_requests Array]
    I -->|No| K[Release Elevator Semaphore]
    J --> L[Increment pending_count]
    L --> K
    K --> M[Call process_elevator]
    E -->|No| M
    M --> N[Sleep 1 second]
    N --> C
```

## Process Elevator Flow

```mermaid
flowchart TD
    A[process_elevator] --> B[Acquire Elevator Semaphore]
    B --> C{Pickup or Dropoff at Current Floor?}
    C -->|Yes| D[Set State = DOOR_OPEN]
    D --> E[Print Door Open Message]
    E --> F{Pickup Request at Current Floor?}
    F -->|Yes| G[Clear pickup_requests[current_floor]]
    G --> H[Process All Pending Requests for This Floor]
    H --> I[For Each Matching Pending Request:]
    I --> J[Mark dropoff_requests[to_floor] = true]
    J --> K[Increment passengers]
    K --> L[Remove from pending_requests Array]
    L --> M{Dropoff Request at Current Floor?}
    F -->|No| M
    M -->|Yes| N[Clear dropoff_requests[current_floor]]
    N --> O[Decrement passengers]
    M -->|No| P[Release Semaphore]
    O --> P
    P --> Q[Sleep 2 seconds - Door Open Time]
    Q --> R[Acquire Semaphore]
    R --> S[Set State = IDLE]
    S --> T[Print Door Closed Message]
    T --> U[Call find_next_stop]
    C -->|No| U
    U --> V{next_stop == -1?}
    V -->|Yes| W[Set State = IDLE, direction = 0]
    W --> X[Print IDLE Message]
    V -->|No| Y{next_stop == current_floor?}
    Y -->|Yes| Z[Release Semaphore & Return]
    Y -->|No| AA{next_stop > current_floor?}
    AA -->|Yes| BB[Set direction = 1, State = MOVING_UP]
    BB --> CC[Increment current_floor]
    CC --> DD[Print Moving UP Message]
    AA -->|No| EE[Set direction = -1, State = MOVING_DOWN]
    EE --> FF[Decrement current_floor]
    FF --> GG[Print Moving DOWN Message]
    DD --> Z
    GG --> Z
    X --> Z
```

## Find Next Stop Algorithm Flow

```mermaid
flowchart TD
    A[find_next_stop] --> B{Request at Current Floor?}
    B -->|Yes| C[Return current_floor]
    B -->|No| D{Direction == 1 (UP)?}
    D -->|Yes| E[Search floors above current_floor]
    E --> F{Found request above?}
    F -->|Yes| G[Return that floor]
    F -->|No| H[Search floors below current_floor]
    D -->|No| I{Direction == -1 (DOWN)?}
    I -->|Yes| J[Search floors below current_floor]
    J --> K{Found request below?}
    K -->|Yes| L[Return that floor]
    K -->|No| M[Search floors above current_floor]
    I -->|No| N{Direction == 0 (IDLE)?}
    N -->|Yes| O[Search all floors]
    O --> P{Found any request?}
    P -->|Yes| Q[Return that floor]
    P -->|No| R[Return -1 (No requests)]
    H --> S{Found request below?}
    S -->|Yes| T[Return that floor]
    S -->|No| R
    M --> U{Found request above?}
    U -->|Yes| V[Return that floor]
    U -->|No| R
    G --> W[End]
    L --> W
    Q --> W
    T --> W
    V --> W
    R --> W
    C --> W
```

## Thread Interaction Diagram

```mermaid
sequenceDiagram
    participant Main as Main Thread
    participant RG as Request Generator
    participant MQ as Message Queue
    participant E1 as Elevator Thread 1
    participant E2 as Elevator Thread 2
    participant SM as Shared Memory
    participant SEM as Semaphores

    Main->>+SM: Initialize System Data
    Main->>+E1: Create Elevator Thread 1
    Main->>+E2: Create Elevator Thread 2
    Main->>+RG: Create Request Generator Thread
    
    RG->>RG: Generate Random Request
    RG->>SM: Read Elevator States
    RG->>RG: Find Best Elevator
    RG->>MQ: Send Request Message
    
    E1->>MQ: Check for Messages (Non-blocking)
    MQ->>E1: Return Request Message
    E1->>SEM: Acquire Elevator Semaphore
    E1->>SM: Update Elevator Data
    E1->>SEM: Release Elevator Semaphore
    E1->>E1: Process Elevator Movement
    
    E2->>MQ: Check for Messages (Non-blocking)
    MQ->>E2: No Message Available
    E2->>E2: Process Elevator Movement
    
    loop Every Second
        E1->>E1: Check Messages & Process
        E2->>E2: Check Messages & Process
    end
    
    RG->>Main: Thread Completion
    Main->>E1: Cancel Thread
    Main->>E2: Cancel Thread
    Main->>SM: Cleanup System
```

## State Transition Diagram for Elevator

```mermaid
stateDiagram-v2
    [*] --> IDLE: System Start
    IDLE --> MOVING_UP: Request Above Current Floor
    IDLE --> MOVING_DOWN: Request Below Current Floor
    MOVING_UP --> DOOR_OPEN: Arrive at Pickup/Dropoff Floor
    MOVING_DOWN --> DOOR_OPEN: Arrive at Pickup/Dropoff Floor
    MOVING_UP --> MOVING_UP: Continue to Next Floor
    MOVING_DOWN --> MOVING_DOWN: Continue to Next Floor
    DOOR_OPEN --> IDLE: Door Close (No More Requests)
    DOOR_OPEN --> MOVING_UP: Door Close (Requests Above)
    DOOR_OPEN --> MOVING_DOWN: Door Close (Requests Below)
    IDLE --> IDLE: No Requests Available
```

## Memory and IPC Structure

```mermaid
graph TB
    subgraph "Process Space"
        MT[Main Thread]
        RT[Request Thread]
        ET1[Elevator Thread 1]
        ET2[Elevator Thread 2]
        ETN[Elevator Thread N]
    end
    
    subgraph "Shared Memory"
        SD[SystemData]
        ED1[ElevatorData 1]
        ED2[ElevatorData 2]
        EDN[ElevatorData N]
        SD --> ED1
        SD --> ED2
        SD --> EDN
    end
    
    subgraph "Message Queue"
        MQ[Message Queue]
        M1[Message Type 1]
        M2[Message Type 2]
        MN[Message Type N]
        MQ --> M1
        MQ --> M2
        MQ --> MN
    end
    
    subgraph "Semaphores"
        SSEM[System Semaphore]
        ESEM1[Elevator Semaphore 1]
        ESEM2[Elevator Semaphore 2]
        ESEMN[Elevator Semaphore N]
    end
    
    RT -.->|Send Request| MQ
    ET1 -.->|Receive Type 1| M1
    ET2 -.->|Receive Type 2| M2
    ETN -.->|Receive Type N| MN
    
    MT -.->|Access| SD
    RT -.->|Access| SD
    ET1 -.->|Access| ED1
    ET2 -.->|Access| ED2
    ETN -.->|Access| EDN
    
    MT -.->|Lock/Unlock| SSEM
    RT -.->|Lock/Unlock| SSEM
    ET1 -.->|Lock/Unlock| ESEM1
    ET2 -.->|Lock/Unlock| ESEM2
    ETN -.->|Lock/Unlock| ESEMN
```

## Key Design Decisions Flow

```mermaid
flowchart TD
    A[Design Decision: Separate Pickup/Dropoff] --> B[Why: Realistic Passenger Flow]
    C[Design Decision: SCAN Algorithm] --> D[Why: Minimize Elevator Movement]
    E[Design Decision: Message Queue per Elevator] --> F[Why: Direct Request Routing]
    G[Design Decision: Semaphore per Elevator] --> H[Why: Fine-grained Locking]
    I[Design Decision: Pending Requests Array] --> J[Why: Two-Phase Request Processing]
    K[Design Decision: Non-blocking Message Check] --> L[Why: Prevent Thread Blocking]
    
    B --> M[Result: Doors only open when needed]
    D --> N[Result: Efficient elevator scheduling]
    F --> O[Result: Scalable request distribution]
    H --> P[Result: Reduced lock contention]
    J --> Q[Result: Proper passenger boarding simulation]
    L --> R[Result: Responsive elevator control]
```
