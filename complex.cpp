#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#define MAX_WORKERS 3
#define SHARED_MEM_SIZE 1024
#define FIFO_PATH "./process_log"
#define MSG_SIZE 100
#define TASK_QUEUE_KEY 0x1234
#define RESULT_QUEUE_KEY 0x5678

// Shared memory structure
struct SharedData {
    int task_count;
    int completed_tasks;
    int results[100];
    bool shutdown_flag;
};

// Message queue structures
struct TaskMessage {
    long msg_type;
    int task_id;
    int data_value;
};

struct ResultMessage {
    long msg_type;
    int task_id;
    int result;
    pid_t worker_pid;
};

// Global variables for IPC
int shmid, semid_mutex, semid_full, semid_empty;
int task_queue_id, result_queue_id;
SharedData* shared_data;
bool signal_received = false;

// Signal handler
void signal_handler(int signo) {
    signal_received = true;
    printf("Process %d received signal %d\n", getpid(), signo);
}

// Semaphore operations
struct sembuf sem_wait = {0, -1, 0};
struct sembuf sem_signal = {0, 1, 0};

void sem_wait_op(int semid) {
    semop(semid, &sem_wait, 1);
}

void sem_signal_op(int semid) {
    semop(semid, &sem_signal, 1);
}

// Thread function for parallel processing within worker processes
void* worker_thread(void* arg) {
    int thread_id = *(int*)arg;
    TaskMessage task;
    ResultMessage result;

    while (!shared_data->shutdown_flag) {
        // Receive task from message queue
        if (msgrcv(task_queue_id, &task, sizeof(task) - sizeof(long), 1, IPC_NOWAIT) > 0) {
            printf("Thread %d in process %d processing task %d\n",
                   thread_id, getpid(), task.task_id);

            // Simulate complex processing (100-300ms)
            usleep((100 + (rand() % 200)) * 1000);

            // Calculate result (simulate some computation)
            int processed_result = task.data_value * task.data_value + thread_id;

            // Update shared memory with synchronization
            sem_wait_op(semid_mutex);
            shared_data->results[shared_data->completed_tasks % 100] = processed_result;
            shared_data->completed_tasks++;
            sem_signal_op(semid_mutex);

            // Send result back via message queue
            result.msg_type = 2;
            result.task_id = task.task_id;
            result.result = processed_result;
            result.worker_pid = getpid();

            msgsnd(result_queue_id, &result, sizeof(result) - sizeof(long), 0);

            // Log to FIFO
            int fifo_fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);
            if (fifo_fd != -1) {
                char log_msg[256];
                snprintf(log_msg, sizeof(log_msg),
                        "Thread %d PID %d: Completed task %d with result %d\n",
                        thread_id, getpid(), task.task_id, processed_result);
                write(fifo_fd, log_msg, strlen(log_msg));
                close(fifo_fd);
            }
        } else {
            usleep(50000); // 50ms
        }
    }

    return NULL;
}

// Worker process function
void worker_process(int worker_id, int pipe_fd[2]) {
    printf("Worker %d (PID: %d) started\n", worker_id, getpid());

    // Set up signal handler
    signal(SIGUSR1, signal_handler);

    // Attach to shared memory
    shared_data = (SharedData*)shmat(shmid, NULL, 0);

    // Close write end of pipe (worker only reads)
    close(pipe_fd[1]);

    // Create worker threads
    pthread_t threads[2];
    int thread_ids[2] = {worker_id * 10 + 1, worker_id * 10 + 2};

    for (int i = 0; i < 2; i++) {
        pthread_create(&threads[i], NULL, worker_thread, &thread_ids[i]);
    }

    // Wait for termination signal from parent via pipe
    char terminate_signal;
    read(pipe_fd[0], &terminate_signal, 1);

    // Set shutdown flag and wait for threads to complete
    shared_data->shutdown_flag = true;

    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    shmdt(shared_data);
    close(pipe_fd[0]);

    printf("Worker %d (PID: %d) terminated\n", worker_id, getpid());
    exit(0);
}

// Logger process function
void logger_process() {
    printf("Logger process (PID: %d) started\n", getpid());

    // Create FIFO
    mkfifo(FIFO_PATH, 0666);

    int fifo_fd = open(FIFO_PATH, O_RDONLY);
    char buffer[256];

    while (true) {
        ssize_t bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("[LOG] %s", buffer);
        } else {
            usleep(100000); // 100ms
        }

        // Check if parent process still exists
        if (getppid() == 1) break; // Parent died
    }

    close(fifo_fd);
    unlink(FIFO_PATH);
    exit(0);
}

int main() {
    printf("=== Complex System Calls Demo ===\n");
    printf("Master process PID: %d\n", getpid());

    // Initialize random seed
    srand(time(NULL));

    // Create shared memory
    shmid = shmget(IPC_PRIVATE, SHARED_MEM_SIZE, IPC_CREAT | 0666);
    shared_data = (SharedData*)shmat(shmid, NULL, 0);
    memset(shared_data, 0, sizeof(SharedData));

    // Create semaphores
    semid_mutex = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    semctl(semid_mutex, 0, SETVAL, 1); // Binary semaphore for mutual exclusion

    // Create message queues
    task_queue_id = msgget(TASK_QUEUE_KEY, IPC_CREAT | 0666);
    result_queue_id = msgget(RESULT_QUEUE_KEY, IPC_CREAT | 0666);

    // Create pipes for worker communication
    int worker_pipes[MAX_WORKERS][2];
    pid_t worker_pids[MAX_WORKERS];

    for (int i = 0; i < MAX_WORKERS; i++) {
        if (pipe(worker_pipes[i]) < 0) {
            perror("pipe");
            exit(1);
        }
    }

    // Fork logger process
    pid_t logger_pid = fork();
    if (logger_pid == 0) {
        logger_process();
    }

    // Fork worker processes
    for (int i = 0; i < MAX_WORKERS; i++) {
        worker_pids[i] = fork();
        if (worker_pids[i] == 0) {
            worker_process(i, worker_pipes[i]);
        } else {
            close(worker_pipes[i][0]); // Parent closes read end
        }
    }

    // Master process: Generate and distribute tasks
    printf("Master process generating tasks...\n");

    TaskMessage task;
    task.msg_type = 1;

    // Generate 20 tasks
    for (int i = 0; i < 20; i++) {
        task.task_id = i;
        task.data_value = rand() % 100 + 1;

        msgsnd(task_queue_id, &task, sizeof(task) - sizeof(long), 0);

        sem_wait_op(semid_mutex);
        shared_data->task_count++;
        sem_signal_op(semid_mutex);

        printf("Generated task %d with data %d\n", i, task.data_value);
        usleep(200000); // 200ms
    }

    // Collect results
    ResultMessage result;
    int results_collected = 0;

    printf("\nCollecting results...\n");

    while (results_collected < 20) {
        if (msgrcv(result_queue_id, &result, sizeof(result) - sizeof(long), 2, 0) > 0) {
            printf("Received result for task %d: %d from worker PID %d\n",
                   result.task_id, result.result, result.worker_pid);
            results_collected++;
        }
    }

    // Send signals to workers
    printf("\nSending termination signals to workers...\n");
    for (int i = 0; i < MAX_WORKERS; i++) {
        kill(worker_pids[i], SIGUSR1);
        char terminate = '1';
        write(worker_pipes[i][1], &terminate, 1);
        close(worker_pipes[i][1]);
    }

    // Wait for all worker processes
    for (int i = 0; i < MAX_WORKERS; i++) {
        wait(NULL);
    }

    // Terminate logger
    kill(logger_pid, SIGTERM);
    wait(NULL);

    // Display final statistics
    printf("\n=== Final Statistics ===\n");
    printf("Total tasks generated: %d\n", shared_data->task_count);
    printf("Total tasks completed: %d\n", shared_data->completed_tasks);

    // Cleanup IPC resources
    shmdt(shared_data);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid_mutex, 0, IPC_RMID, 0);
    msgctl(task_queue_id, IPC_RMID, NULL);
    msgctl(result_queue_id, IPC_RMID, NULL);

    printf("Demo completed successfully!\n");
    return 0;
}
