#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>

#define NUM_READERS 3
#define NUM_WRITES 5
#define NUM_READS_PER_READER 3

struct shared_data {
    int shared_resource;
    int reader_count;
};

// Global variables for cleanup
int shm_id;
int sem_id;

// union semun {
//     int val;
//     struct semid_ds *buf;
//     unsigned short *array;
//     struct seminfo *__buf;
// };

#define SEM_MUTEX 0  // Mutex for reader_count
#define SEM_WRITE 1  // Semaphore for writers

void sem_wait(int sem_id, int sem_num) {
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    if (semop(sem_id, &sb, 1) == -1) {
        perror("sem_wait");
        exit(1);
    }
}

void sem_signal(int sem_id, int sem_num) {
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    if (semop(sem_id, &sb, 1) == -1) {
        perror("sem_signal");
        exit(1);
    }
}

void cleanup_resources() {
    // Remove shared memory
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID");
    }

    // Remove semaphores
    union semun sem_union;
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1) {
        perror("semctl IPC_RMID");
    }

    printf("Resources cleaned up.\n");
}

void signal_handler(int sig) {
    printf("\nReceived signal %d. Cleaning up...\n", sig);
    cleanup_resources();
    exit(0);
}

void reader_process(int reader_id, shared_data* data) {
    printf("Reader %d started (PID: %d)\n", reader_id, getpid());

    for (int i = 0; i < NUM_READS_PER_READER; i++) {
        // Entry protocol for readers
        sem_wait(sem_id, SEM_MUTEX);
        data->reader_count++;
        if (data->reader_count == 1) {
            // First reader blocks writers
            sem_wait(sem_id, SEM_WRITE);
        }
        sem_signal(sem_id, SEM_MUTEX);

        // Critical section - reading
        printf("Reader %d: Reading shared_resource = %d (read #%d)\n",
               reader_id, data->shared_resource, i + 1);
        sleep(1); // Simulate reading time

        // Exit protocol for readers
        sem_wait(sem_id, SEM_MUTEX);
        data->reader_count--;
        if (data->reader_count == 0) {
            // Last reader allows writers
            sem_signal(sem_id, SEM_WRITE);
        }
        sem_signal(sem_id, SEM_MUTEX);

        sleep(1); // Wait before next read
    }

    printf("Reader %d finished (PID: %d)\n", reader_id, getpid());
}

void writer_process(shared_data* data) {
    printf("Writer started (PID: %d)\n", getpid());

    for (int i = 0; i < NUM_WRITES; i++) {
        // Writer entry protocol
        sem_wait(sem_id, SEM_WRITE);

        // Critical section - writing
        data->shared_resource += 10;
        printf("Writer: Writing shared_resource = %d (write #%d)\n",
               data->shared_resource, i + 1);
        sleep(2); // Simulate writing time

        // Writer exit protocol
        sem_signal(sem_id, SEM_WRITE);

        sleep(1); // Wait before next write
    }

    printf("Writer finished (PID: %d)\n", getpid());
}

int main() {
    printf("Starting Reader-Writer synchronization with fork()\n");
    printf("Configuration: 1 Writer, %d Readers\n", NUM_READERS);
    printf("Writer will write %d times, each reader will read %d times\n\n",
           NUM_WRITES, NUM_READS_PER_READER);

    // Set up signal handlers for cleanup
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Create shared memory
    key_t shm_key = ftok(__FILE__, 'A');
    if (shm_key == -1) {
        perror("ftok for shared memory");
        exit(1);
    }

    shm_id = shmget(shm_key, sizeof(shared_data), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    shared_data* data = (shared_data*)shmat(shm_id, NULL, 0);
    if (data == (shared_data*)-1) {
        perror("shmat");
        exit(1);
    }

    // Initialize shared data
    data->shared_resource = 0;
    data->reader_count = 0;

    // Create semaphores
    key_t sem_key = ftok(__FILE__, 'B');
    if (sem_key == -1) {
        perror("ftok for semaphores");
        exit(1);
    }

    sem_id = semget(sem_key, 2, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    // Initialize semaphores
    union semun sem_union;
    sem_union.val = 1;
    if (semctl(sem_id, SEM_MUTEX, SETVAL, sem_union) == -1) {
        perror("semctl SEM_MUTEX");
        exit(1);
    }
    if (semctl(sem_id, SEM_WRITE, SETVAL, sem_union) == -1) {
        perror("semctl SEM_WRITE");
        exit(1);
    }

    printf("Shared memory and semaphores initialized.\n\n");

    // Fork reader processes
    pid_t reader_pids[NUM_READERS];
    for (int i = 0; i < NUM_READERS; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork for reader");
            exit(1);
        } else if (pid == 0) {
            // Child process - reader
            reader_process(i + 1, data);
            shmdt(data);
            exit(0);
        } else {
            // Parent process - store child PID
            reader_pids[i] = pid;
        }
    }

    // Fork writer process
    pid_t writer_pid = fork();
    if (writer_pid == -1) {
        perror("fork for writer");
        exit(1);
    } else if (writer_pid == 0) {
        // Child process - writer
        writer_process(data);
        shmdt(data);
        exit(0);
    }

    // Parent process - wait for all children to complete
    printf("Parent process (PID: %d) waiting for all children to complete...\n\n", getpid());

    // Wait for writer
    int status;
    waitpid(writer_pid, &status, 0);
    printf("Writer process completed.\n");

    // Wait for all readers
    for (int i = 0; i < NUM_READERS; i++) {
        waitpid(reader_pids[i], &status, 0);
        printf("Reader %d process completed.\n", i + 1);
    }

    printf("\nAll processes completed successfully.\n");
    printf("Final shared_resource value: %d\n", data->shared_resource);

    // Detach shared memory
    shmdt(data);

    // Clean up resources
    cleanup_resources();

    return 0;
}
