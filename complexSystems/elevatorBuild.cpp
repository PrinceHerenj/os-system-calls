#include <cstddef>
#include <cstdio>
#include <ctime>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define MAX_ELEVATORS 5
#define MAX_FLOORS 10
#define KEY_BASE 1234

typedef enum {
    IDLE,
    MOVING_UP,
    MOVING_DOWN,
    DOOR_OPEN
} ElevatorState ;

typedef struct {
    long mtype;
    int from_floor;
    int to_floor;
    int elevator_id;
    int request_id;
} RequestMessage ;

typedef struct {
    int from_floor;
    int to_floor;
    int request_id;
} PendingRequest ;

typedef struct {
    int current_floor;
    ElevatorState state;
    int passengers;
    bool pickup_requests[MAX_FLOORS];   // floors where people are waiting
    bool dropoff_requests[MAX_FLOORS];  // floors where passengers want to get off
    int direction;                      // 1 for up, -1 for down, 0 for idle
    PendingRequest pending_requests[20]; // requests waiting to be processed
    int pending_count;
} ElevatorData ;

typedef struct {
    ElevatorData elevators[MAX_ELEVATORS];
    int num_elevators;
    int num_floors;
    int total_requests;
} SystemData ;

int shmid;
int msgid;
int system_semid;
int elevator_semid;
SystemData *shared_data;
int num_elevators;
int num_floors;

void *elevator_thread(void *arg);
void *request_generator(void *arg);
int find_best_elevator(int from_floor, int to_floor);
void process_elevator(int elevator_id);
void initialize_system(int n_elevators, int n_floors);
void cleanup_system();
void sem_wait(int semid, int sem_num);
void sem_signal(int semid, int sem_num);
int create_semaphore_set(key_t key, int num_sems, int initial_value);

void sem_wait(int semid, int sem_num){
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = -1;
    sb.sem_flg = 0;

    if (semop(semid, &sb, 1) == -1) {
        perror("semop wait failed");
        exit(1);
    }
}

void sem_signal(int semid, int sem_num) {
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = 1;
    sb.sem_flg = 0;

    if (semop(semid, &sb, 1) == -1) {
        perror("semop signal failed");
        exit(1);
    }
}

int create_semaphore_set(key_t key, int num_sems, int initial_value) {
    int semid = semget(key, num_sems, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }

    union semun arg;
    arg.val = initial_value;
    for (int i = 0; i < num_sems; i++) {
        if (semctl(semid, i, SETVAL, arg) == -1) {
            perror("semctl SETVAL failed");
            exit(1);
        }
    }

    return semid;
}

void initialize_system(int n_elevators, int n_floors) {
    key_t shm_key = ftok(".", KEY_BASE);
    key_t msg_key = ftok(".", KEY_BASE + 1);
    key_t sys_sem_key = ftok(".", KEY_BASE + 2);
    key_t elev_sem_key = ftok(".", KEY_BASE + 3);

    num_elevators = n_elevators;
    num_floors = n_floors;

    shmid = shmget(shm_key, sizeof(SystemData), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }

    shared_data = (SystemData *)shmat(shmid, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    msgid = msgget(msg_key, IPC_CREAT | 0666);
    if (msgid < 0) {
        perror("msgget failed");
        exit(1);
    }

    system_semid = create_semaphore_set(sys_sem_key, 1, 1);
    elevator_semid = create_semaphore_set(elev_sem_key, n_elevators, 1);

    shared_data->num_elevators = n_elevators;
    shared_data->num_floors = n_floors;
    shared_data->total_requests = 0;

    for (int i = 0; i < n_elevators; i++) {
        shared_data->elevators[i].current_floor = 0;
        shared_data->elevators[i].state = IDLE;
        shared_data->elevators[i].passengers = 0;
        shared_data->elevators[i].direction = 0;
        shared_data->elevators[i].pending_count = 0;

        for(int j = 0; j < MAX_FLOORS; j++) {
            shared_data->elevators[i].pickup_requests[j] = false;
            shared_data->elevators[i].dropoff_requests[j] = false;
        }
    }

    printf("System initialized: %d elevators, %d floors\n", n_elevators, n_floors);
}

int find_best_elevator(int from_floor, int to_floor) {
    int best = 0;
    int min_distance = num_floors + 1;

    sem_wait(system_semid, 0);
    for (int i = 0; i < num_elevators; i++) {
        sem_wait(elevator_semid, i);

        int distance = abs(shared_data->elevators[i].current_floor - from_floor);

        // Prefer idle elevators
        if (shared_data->elevators[i].state == IDLE) {
            distance -= 5;
        }

        // Prefer elevators moving in the right direction
        int required_direction = (to_floor > from_floor) ? 1 : -1;
        if (shared_data->elevators[i].direction == required_direction &&
            ((required_direction == 1 && shared_data->elevators[i].current_floor <= from_floor) ||
             (required_direction == -1 && shared_data->elevators[i].current_floor >= from_floor))) {
            distance -= 3;
        }

        if (distance < min_distance) {
            min_distance = distance;
            best = i;
        }

        sem_signal(elevator_semid, i);
    }
    sem_signal(system_semid, 0);

    return best;
}

int find_next_stop(ElevatorData *elevator) {
    // First check current floor
    if (elevator->pickup_requests[elevator->current_floor] ||
        elevator->dropoff_requests[elevator->current_floor]) {
        return elevator->current_floor;
    }

    // Then check in current direction
    if (elevator->direction == 1) {
        for (int i = elevator->current_floor + 1; i < num_floors; i++) {
            if (elevator->pickup_requests[i] || elevator->dropoff_requests[i]) {
                return i;
            }
        }
    } else if (elevator->direction == -1) {
        for (int i = elevator->current_floor - 1; i >= 0; i--) {
            if (elevator->pickup_requests[i] || elevator->dropoff_requests[i]) {
                return i;
            }
        }
    }

    // If no requests in current direction, check opposite direction
    if (elevator->direction == 1) {
        for (int i = elevator->current_floor - 1; i >= 0; i--) {
            if (elevator->pickup_requests[i] || elevator->dropoff_requests[i]) {
                return i;
            }
        }
    } else if (elevator->direction == -1) {
        for (int i = elevator->current_floor + 1; i < num_floors; i++) {
            if (elevator->pickup_requests[i] || elevator->dropoff_requests[i]) {
                return i;
            }
        }
    }

    // If direction is 0 (idle), check all floors
    if (elevator->direction == 0) {
        for (int i = 0; i < num_floors; i++) {
            if (elevator->pickup_requests[i] || elevator->dropoff_requests[i]) {
                return i;
            }
        }
    }

    return -1; // No requests
}

void process_elevator(int elevator_id) {
    ElevatorData *elevator = &shared_data->elevators[elevator_id];

    sem_wait(elevator_semid, elevator_id);

    // Check if we should stop at current floor
    if (elevator->pickup_requests[elevator->current_floor] ||
        elevator->dropoff_requests[elevator->current_floor]) {

        elevator->state = DOOR_OPEN;
        printf("Elevator %d: DOOR OPEN at floor %d", elevator_id, elevator->current_floor);

        if (elevator->pickup_requests[elevator->current_floor]) {
            printf(" (PICKUP)");
            elevator->pickup_requests[elevator->current_floor] = false;

            // Process all pending requests for this floor
            for (int i = 0; i < elevator->pending_count; i++) {
                if (elevator->pending_requests[i].from_floor == elevator->current_floor) {
                    elevator->dropoff_requests[elevator->pending_requests[i].to_floor] = true;
                    elevator->passengers++;
                    printf("\n    Passenger %d boarded, going to floor %d",
                           elevator->pending_requests[i].request_id,
                           elevator->pending_requests[i].to_floor);

                    // Remove this request from pending list
                    for (int j = i; j < elevator->pending_count - 1; j++) {
                        elevator->pending_requests[j] = elevator->pending_requests[j + 1];
                    }
                    elevator->pending_count--;
                    i--; // Adjust index since we removed an element
                }
            }
        }

        if (elevator->dropoff_requests[elevator->current_floor]) {
            printf(" (DROPOFF)");
            elevator->dropoff_requests[elevator->current_floor] = false;
            elevator->passengers--;
        }

        printf("\n");
        sem_signal(elevator_semid, elevator_id);

        sleep(2); // Door open time

        sem_wait(elevator_semid, elevator_id);
        elevator->state = IDLE;
        printf("Elevator %d: DOOR CLOSED at floor %d (passengers: %d)\n",
               elevator_id, elevator->current_floor, elevator->passengers);
    }

    // Find next destination
    int next_stop = find_next_stop(elevator);

    if (next_stop == -1) {
        // No more requests
        elevator->state = IDLE;
        elevator->direction = 0;
        printf("Elevator %d: IDLE at floor %d\n", elevator_id, elevator->current_floor);
    } else if (next_stop == elevator->current_floor) {
        // Already handled above
    } else {
        // Move towards next stop
        if (next_stop > elevator->current_floor) {
            elevator->direction = 1;
            elevator->state = MOVING_UP;
            elevator->current_floor++;
            printf("Elevator %d: Moving UP to floor %d (target: %d)\n",
                   elevator_id, elevator->current_floor, next_stop);
        } else {
            elevator->direction = -1;
            elevator->state = MOVING_DOWN;
            elevator->current_floor--;
            printf("Elevator %d: Moving DOWN to floor %d (target: %d)\n",
                   elevator_id, elevator->current_floor, next_stop);
        }
    }

    sem_signal(elevator_semid, elevator_id);
}

void *elevator_thread(void *arg) {
    int elevator_id = *(int *) arg;
    free(arg);

    printf("Elevator %d thread started\n", elevator_id);

    while(1) {
        RequestMessage msg;
        if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), elevator_id + 1, IPC_NOWAIT) > 0) {
            printf("Elevator %d: Received request from floor %d to floor %d\n",
                   elevator_id, msg.from_floor, msg.to_floor);

            sem_wait(elevator_semid, elevator_id);

            // Mark pickup request and add to pending requests
            shared_data->elevators[elevator_id].pickup_requests[msg.from_floor] = true;

            // Add to pending requests list
            if (shared_data->elevators[elevator_id].pending_count < 20) {
                PendingRequest *pending = &shared_data->elevators[elevator_id].pending_requests[shared_data->elevators[elevator_id].pending_count];
                pending->from_floor = msg.from_floor;
                pending->to_floor = msg.to_floor;
                pending->request_id = msg.request_id;
                shared_data->elevators[elevator_id].pending_count++;
            }

            sem_signal(elevator_semid, elevator_id);
        }

        process_elevator(elevator_id);
        sleep(1);
    }

    return NULL;
}

void *request_generator(void *arg) {
    int num_requests = *(int *) arg;
    free(arg);

    printf("Request generator started. Will generate %d requests.\n", num_requests);
    sleep(2);

    for (int i = 0; i< num_requests; i++) {
        RequestMessage msg;
        msg.from_floor = rand() % num_floors;
        msg.to_floor = rand() % num_floors;

        while (msg.from_floor == msg.to_floor) {
            msg.to_floor = rand() % num_floors;
        }

        int elevator_id = find_best_elevator(msg.from_floor, msg.to_floor);
        msg.mtype = elevator_id + 1;
        msg.elevator_id = elevator_id;
        msg.request_id = i;

        printf("\n>> NEW REQUEST %d: Floor %d -> Floor %d (assigned to Elevator %d)\n",
               i, msg.from_floor, msg.to_floor, elevator_id);

        if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) < 0) {
            perror("msgsnd failed");
        }

        sem_wait(system_semid, 0);
        shared_data->total_requests++;
        sem_signal(system_semid, 0);

        sleep(2 + rand() % 3);
    }

    printf("\nRequest generator finished. All requests sent.\n");
    return NULL;
}

void cleanup_system() {
    printf("\nCleaning up system\n");

    if (semctl(system_semid, 0, IPC_RMID) == -1) {
        perror("semctl IPC_RMID system_semid failed");
    }
    if (semctl(elevator_semid, 0, IPC_RMID) == -1) {
        perror("semctl IPC_RMID elevator_semid failed");
    }

    shmdt(shared_data);
    shmctl(shmid, IPC_RMID, NULL);
    msgctl(msgid, IPC_RMID, NULL);

    printf("Cleanup complete.\n");
}

int main(int argc, char *argv[]) {
    int n_elevators = 2;
    int n_floors = 6;
    int n_requests = 3;

    if (argc >= 3) {
        n_elevators = atoi(argv[1]);
        n_floors = atoi(argv[2]);
    }
    if (argc >= 4) {
        n_requests = atoi(argv[3]);
    }

    if (n_elevators > MAX_ELEVATORS || n_floors > MAX_FLOORS) {
        printf("Error: Too many elevators or floors (max: %d elevators, %d floors)\n",
               MAX_ELEVATORS, MAX_FLOORS);
        return 1;
    }

    printf("Multi-Elevator Control System\n");
    printf("Elevators: %d, Floors: %d, Requests: %d\n\n", n_elevators, n_floors, n_requests);

    srand(time(NULL));

    initialize_system(n_elevators, n_floors);

    pthread_t elevator_threads[MAX_ELEVATORS];
    for (int i = 0; i < n_elevators; i++) {
        int *id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&elevator_threads[i], NULL, elevator_thread, id);
    }

    pthread_t request_thread;
    int *req_count = (int *)malloc(sizeof(int));
    *req_count = n_requests;
    pthread_create(&request_thread, NULL, request_generator, req_count);

    pthread_join(request_thread, NULL);

    printf("\nWaiting for elevators to finish processing\n");
    sleep(20);

    for (int i = 0; i < n_elevators; i++) {
        pthread_cancel(elevator_threads[i]);
    }

    cleanup_system();
    printf("\nSimulation complete!\n");
    return 0;
}
