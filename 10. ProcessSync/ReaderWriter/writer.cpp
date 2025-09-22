#include <cstddef>
#include <cstdio>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHM_SIZE 1024
#define NUM_WRITES 10

struct shared_data {
    int shared_resource;
    int reader_count;
};

int sem_id;
#define SEM_MUTEX 0
#define SEM_WRITE 1

void sem_wait(int sem_num) {
    sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    semop(sem_id, &sb, 1);
}

void sem_signal(int sem_num) {
    sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(sem_id, &sb, 1);
}

int main() {
    key_t shm_key = ftok("./writer.cpp", 'A');
    key_t sem_key = ftok("./writer.cpp", 'B');

    int shm_id = shmget(shm_key, sizeof(shared_data), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    shared_data* data = (shared_data *) shmat(shm_id, NULL, 0);
    if (data == (shared_data *)-1) {
        perror("shmat");
        exit(1);
    }

    sem_id = semget(sem_key, 2, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    semun sem_union;
    sem_union.val = 1;
    semctl(sem_id, SEM_MUTEX, SETVAL, sem_union);
    sem_union.val = 1;
    semctl(sem_id, SEM_WRITE, SETVAL, sem_union);

    for (int i = 0; i < NUM_WRITES; i++) {
        sem_wait(SEM_WRITE);
        data->shared_resource++;
        printf("Writer: Writing shared_shared = %d\n", data->shared_resource);
        usleep(500000);
        sem_signal(SEM_WRITE);
        usleep(500000);
    }

    shmdt(data);
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID, sem_union);

    return 0;
}
