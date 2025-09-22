#include <iostream>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>

#define P(s) semop(s, &pop, 1)
#define V(s) semop(s, &vop, 1)

int main() {
    sembuf pop, vop;

    int semid = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);

    semctl(semid, 0, SETVAL, 0);
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = 1; vop.sem_op = 1;

    if (fork() ==  0) {
        P(semid);
        sleep(2);
        std::cout << "Child exiting" << std::endl;
    } else {
        V(semid);
        wait(NULL);

        semctl(semid, 0, IPC_RMID, 0);
    }
}
