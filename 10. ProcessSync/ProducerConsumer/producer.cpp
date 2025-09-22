#include <cstdio>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
using namespace std;

#define BUF_SIZE 5
#define NUM_ITEMS 20

int *buffer;
int *in, *out;

int sem_id;
#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL 2

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

    int shm_id = shmget(shm_key, sizeof(int) * (BUF_SIZE + 2), IPC_CREAT | 0666);
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
    *in = 0;
    *out = 0;

    sem_id = semget(sem_key, 3, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    semun sem_union;
    sem_union.val = 1;
    semctl(sem_id, SEM_MUTEX, SETVAL, sem_union);

    sem_union.val = BUF_SIZE;
    semctl(sem_id, SEM_EMPTY, SETVAL, sem_union);

    sem_union.val = 0;
    semctl(sem_id, SEM_FULL, SETVAL, sem_union);

    for (int i = 1; i <= NUM_ITEMS; i++) {
        sem_wait(SEM_EMPTY);
        sem_wait(SEM_MUTEX);

        buffer[*in + 2] = i;
        printf("Produced: %d at index %d\n", buffer[*in + 2], *in);
        *in = (*in + 1) % BUF_SIZE;

        sem_signal(SEM_MUTEX);
        sem_signal(SEM_FULL);
        usleep(rand() % 100000);
    }

    // shmctl(shm_id, IPC_RMID, NULL);
    // semctl(sem_id, 0, IPC_RMID, sem_union);
    return 0;
}
