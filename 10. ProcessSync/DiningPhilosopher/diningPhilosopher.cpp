
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <sys/_types/_pid_t.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

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

void philosopher(int id, int semids[5]) {
    for (int meals = 0; meals < 2; meals++) {
        cout << "Philosopher " << id << " is Thinking.\n";
        usleep(rand() % 1000000);

        sem_wait(semids[id]);

        sem_wait(semids[(id + 1) % 5]);

        cout << "Philosopher " << id << " is eating.\n";
        usleep(rand() % 1000000);

        sem_signal(semids[(id + 1) % 5]);
        sem_signal(semids[id]);

        cout << "Philosopher " << id << " is done eating and back to thinking.\n";
    }
    cout << "Philosopher " << id << " has finished all meals.\n";
}

int main() {
    key_t sem_key = ftok("./semfile", 75);

    int semids[5];
    for (int i = 0; i < 5; i++) {
        semids[i] = semget(sem_key + i, 1, IPC_CREAT | 0666);
        if (semids[i] == -1) {
            perror("semget");
            exit(1);
        }

        semun arg;
        arg.val = 1;
        if (semctl(semids[i], 0, SETVAL, arg) == -1) {
            perror("semctl");
            exit(1);
        }
    }

    pid_t pids[5];
    for (int i = 0; i < 5; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            philosopher(i, semids);
            exit(0);
        } else if (pids[i] == -1) {
            perror("fork");
            exit(1);
        }
    }

    for (int i = 0; i < 5; i++) {
        waitpid(pids[i], NULL, 0);
    }

    for (int i = 0; i < 5; i++) {
        semctl(semids[i], 0, IPC_RMID, 0);
    }

    cout << "All philosophers have finished dining.\n";
    return 0;
}
