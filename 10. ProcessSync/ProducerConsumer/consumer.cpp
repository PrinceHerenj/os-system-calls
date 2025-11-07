#include <cstdio>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define BUF_SIZE 5
#define NUM_ITEMS 20

int *buffer;
int *in, *out;

int sem_id;
#define SEM_MUTEX_ID 0
#define SEM_EMPTY_ID 1
#define SEM_FULL_ID 2

void sem_wait(int sem_num) {
    sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    if (semop(sem_id, &sb, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

void sem_signal(int sem_num) {
    sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    if (semop(sem_id, &sb, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

int main() {
    key_t shm_key = ftok("./producer.cpp", 'R');
    key_t sem_key = ftok("./producer.cpp", 'S');

    int shm_id = shmget(shm_key, sizeof(int) * (BUF_SIZE + 2), 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    buffer = (int *) shmat(shm_id, NULL, 0);
    if (buffer == (int *)-1) {
        perror("shmat");
        exit(1);
    }

    in = &buffer[0];
    out = &buffer[1];

    sem_id = semget(sem_key, 3, 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    for (int i = 1; i <= NUM_ITEMS; i++) {
        sem_wait(SEM_FULL_ID);
        sem_wait(SEM_MUTEX_ID);

        int item = buffer[*out + 2];
        printf("Consumed: %d at index %d\n", item, *out);
        *out = (*out + 1) % BUF_SIZE;
        sem_signal(SEM_MUTEX_ID);
        sem_signal(SEM_EMPTY_ID);
        usleep(rand() % 150000);
    }
    return 0;
}
