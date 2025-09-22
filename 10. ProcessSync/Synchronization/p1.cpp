#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
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

    int semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    union semun arg;
    arg.val = 1;

    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl");
        exit(1);
    }

    // Initialize shared memory values
    shm->x = 0;
    shm->y = 0;

    while (true) {
        sem_wait(semid);

        shm->x = shm->y + 1;
        cout << "P1: y = " << shm->y << " -> x = " << shm->x << endl;

        sem_signal(semid);
        usleep(500000);
    }

    shmdt(shm);
    return 0;
}
