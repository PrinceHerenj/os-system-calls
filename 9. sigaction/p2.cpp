#include <csignal>
#include <cstdio>
#include <sys/_types/_pid_t.h>
#include <sys/signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 0x1234

void hdfn(int sig, siginfo_t * si, void * context) {
    printf("received signal %d from PID: %d\n", sig, si->si_pid);
}

int main() {
    int shmid = shmget(SHM_KEY, sizeof(pid_t), IPC_CREAT | 0666);

    pid_t * shm_ptr = (pid_t *) shmat(shmid, NULL, 0);

    *shm_ptr = getpid();
    printf("PID %d written to shred memory\n", *shm_ptr);

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = hdfn;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
    printf("Waiting for signal\n");


    while (1) {
        pause();
    }

    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);
}
