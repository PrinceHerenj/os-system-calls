#include <cstdio>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

struct SharedMemory {
    int x;
    int y;
};


void sem_wait(int semid) {
    sembuf sb = {0, -1, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

void sem_signal(int semid) {
    sembuf sb = {0, 1, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(1);
    }
}


int main() {
    key_t shm_key = ftok("shmfile", 65);
    key_t sem_key = ftok("semfile", 75);

    int shm_id = shmget(shm_key, sizeof(SharedMemory), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    SharedMemory* shm = (SharedMemory*) shmat(shm_id, nullptr, 0);
    if (shm == (SharedMemory *)-1) {
        perror("shmat");
        exit(1);
    }

    int semid = semget(sem_key, 1, 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    while (true) {
        sem_wait(semid);

        shm->y = shm->x + 1;
        cout << "P2: x = " << shm->x << " -> y = " << shm->y << endl;

        sem_signal(semid);
        usleep(500000);
    }
    shmdt(shm);
    return 0;
}
