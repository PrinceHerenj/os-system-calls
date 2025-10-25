#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 20
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 3

int *buffer;
int *in, *out;

int sem_id;
#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL 2


void sem_wait(int sem_num) {
    struct sembuf sop;
    sop.sem_num = sem_num;
    sop.sem_op = -1;
    sop.sem_flg = 0;
    semop(sem_id, &sop, 1);
}

void sem_signal(int sem_num) {
    struct sembuf sop;
    sop.sem_num = sem_num;
    sop.sem_op = 1;
    sop.sem_flg = 0;
    semop(sem_id, &sop, 1);
}

void producer(int id) {
    for (int i = 1; i <= NUM_ITEMS; i++) {
        int item = id * 100 + i;

        sem_wait(SEM_EMPTY);  // Wait for empty slot
        sem_wait(SEM_MUTEX);  // Enter critical section

        // FIXED: Use proper indexing - buffer[0] is 'in', buffer[1] is 'out'
        // Actual buffer starts at buffer[2]
        int pos = *in;
        buffer[2 + pos] = item;
        printf("Producer %d: Produced %d at position %d\n", id, item, pos);
        *in = (*in + 1) % BUFFER_SIZE;

        sem_signal(SEM_MUTEX);  // Exit critical section
        sem_signal(SEM_FULL);   // Signal item available

        usleep(100000);  // 100ms
    }
    printf("Producer %d: Finished\n", id);
}


void consumer(int id) {
    for (int i = 1; i <= NUM_ITEMS; i++) {
        sem_wait(SEM_FULL);   // Wait for item
        sem_wait(SEM_MUTEX);  // Enter critical section

        // FIXED: Use proper indexing
        int pos = *out;
        int item = buffer[2 + pos];
        printf("Consumer %d: Consumed %d from position %d\n", id, item, pos);
        *out = (*out + 1) % BUFFER_SIZE;

        sem_signal(SEM_MUTEX);  // Exit critical section
        sem_signal(SEM_EMPTY);  // Signal slot available

        usleep(150000);  // 150ms
    }
    printf("Consumer %d: Finished\n", id);
}

int main() {
    key_t shm_key = ftok("./shmkey", 'R');
    key_t sem_key = ftok("./semkey", 'S');

    int shm_id = shmget(shm_key, sizeof(int) * (BUFFER_SIZE + 2), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    buffer = (int *) shmat(shm_id, NULL, 0);
    if(buffer == (int *)-1) {
        perror("shmat");
        exit(1);
    }

    in = &buffer[0];
    out = &buffer[1];

    *in = 0;
    *out = 0;

    sem_id = semget(sem_key, 3, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    union semun sem_union;
    sem_union.val = 1;
    semctl(sem_id, SEM_MUTEX, SETVAL, sem_union);

    sem_union.val = BUFFER_SIZE;
    semctl(sem_id, SEM_EMPTY, SETVAL, sem_union);

    sem_union.val = 0;
    semctl(sem_id, SEM_FULL, SETVAL, sem_union);

    pid_t pid;
    for (int i = 0 ; i < NUM_PRODUCERS; i++) {
        pid = fork();
        if (pid == 0) {
            producer(i + 1);
            exit(0);
        }
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pid = fork();
        if (pid == 0) {
            consumer(i + 1);
            exit(0);
        }
    }

    for (int i = 0; i < NUM_PRODUCERS + NUM_CONSUMERS; i++) {
        wait(NULL);
    }

    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID, sem_union);

    return 0;
}
