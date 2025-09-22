#include <cstddef>
#include <cstdio>
#include <stdlib.h>
#include <sys/_types/_key_t.h>
#include <sys/_types/_pid_t.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define NUM_READERS 5
#define NUM_WRITES 5
#define NUM_READS_PER_READER 5

struct shared_data {
    int shared_resource;
    int reader_count;
};

int shm_id;
int sem_id;

#define SEM_MUTEX 0
#define SEM_WRITE 1

void sem_wait(int sem_id, int sem_num) {
    sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = -1;
    sb.sem_flg = 0;

    if (semop(sem_id, &sb, 1) == -1) {
        perror("sem_wait");
        exit(1);
    }
}

void sem_signal(int sem_id, int sem_num) {
    sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = 1;
    sb.sem_flg = 0;

    if (semop(sem_id, &sb, 1) == -1) {
        perror("sem_signal");
        exit(1);
    }
}

void reader_process_cs(int reader_id, shared_data* data) {
    printf("Reader %d started (PID: %d)\n", reader_id, getpid());

    for (int i = 0; i < NUM_READS_PER_READER; i++) {
        sem_wait(sem_id, SEM_MUTEX);
        data->reader_count++;
        if (data->reader_count == 1) sem_wait(sem_id, SEM_WRITE);
        sem_signal(sem_id, SEM_MUTEX);

        printf("Reader %d: Reading shared_resource = %d (read #%d)\n", reader_id, data->shared_resource, i + 1);
        sleep(1);

        sem_wait(sem_id, SEM_MUTEX);
        data->reader_count--;
        if (data->reader_count == 0) sem_signal(sem_id, SEM_WRITE);
        sem_signal(sem_id, SEM_MUTEX);

        sleep(1);
    }

    printf("Reader %d finished (PID: %d)\n", reader_id, getpid());
}

int main() {
    key_t shm_key = ftok(__FILE__, 'A');
    if (shm_key == -1) {
        perror("ftok");
        exit(1);
    }

    shm_id = shmget(shm_key, sizeof(shared_data), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    shared_data* data = (shared_data*) shmat(shm_id, NULL, 0);
    if (data == (shared_data*)-1) {
        perror("shmat");
        exit(1);
    }

    data->shared_resource = 0;
    data->reader_count = 0;

    key_t sem_key = ftok(__FILE__, 'B');
    if (sem_key == -1) {
        perror("ftok");
        exit(1);
    }

    sem_id = semget(sem_key, 2, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    semun su;
    su.val = 1;
    if (semctl(sem_id, SEM_MUTEX, SETVAL, su) == -1) {
        perror("semctl");
    }
    if (semctl(sem_id, SEM_WRITE, SETVAL, su) == -1) {
        perror("semctl");
        exit(1);
    }

    pid_t reader_pids[NUM_READERS];
    for (int i = 0; i < NUM_READERS; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            reader_process_cs(i + 1, data);
            shmdt(data);
            exit(0);
        } else {
            reader_pids[i] = pid;
        }
    }

    printf("Parent Process (PID: %d) acting as writer\n", getpid());

    sleep(1);

    for (int i = 0; i < NUM_WRITES; i++) {
        // Writer entry protocol
        sem_wait(sem_id, SEM_WRITE);

        // Critical section - writing
        data->shared_resource += 10;
        printf("Writer (Parent): Writing shared_resource = %d (write #%d)\n",
                data->shared_resource, i + 1);
        sleep(2); // Simulate writing time

        // Writer exit protocol
        sem_signal(sem_id, SEM_WRITE);

        sleep(1); // Wait before next write
    }

    for (int i = 0; i < NUM_READERS; i++) {
        int status;
        waitpid(reader_pids[i], &status, 0);
        printf("Reader %d completed.\n", i + 1);
    }

    shmdt(data);

    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID, su);

    return 0;

}
