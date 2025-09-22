
#include <cstdio>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>

#define P(s) semop(s, &pop, 1)
#define V(s) semop(s, &vop, 1)

int main() {
    int count = 20, status;

    int semid1, semid2;
    sembuf pop, vop;

    semid1 = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
    semid2 = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);

    semctl(semid1, 0, SETVAL, 0);
    semctl(semid2, 0, SETVAL, 1);

    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1; vop.sem_op = 1;

    if (fork() == 0) {
        FILE * fp;
        int data;

        while (count) {
            P(semid1);
            fp = fopen("./datafile", "r");
            fscanf(fp, "%d", &data);
            printf("reads %d\n", data);
            fclose(fp);
            V(semid2);
            count--;
        }
    } else {
        FILE * fp;
        int data = 0;
        while (count) {
            sleep(1);
            P(semid2);
            fp = fopen("./datafile", "w");
            fprintf(fp, "%d\n", data);
            printf("writes %d\n", data);
            data++;
            fclose(fp);
            V(semid1);
            count--;
        }

        wait(NULL);

        semctl(semid1, 0, IPC_RMID, 0);
        semctl(semid2, 0, IPC_RMID, 0);
    }
}
