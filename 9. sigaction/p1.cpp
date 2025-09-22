#include <cstddef>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <signal.h>
#include <cstdio>
#include <unistd.h>

#define SHM_KEY 0x1234

int main() {
    int shmid = shmget(SHM_KEY, sizeof(pid_t), 0666 | IPC_CREAT);
    void *shm_ptr = shmat(shmid, NULL, 0);
    pid_t p2_pid = *(pid_t *)shm_ptr;

    kill(p2_pid, SIGUSR1);
    printf("Sent SIGUSR1 to P2(PID %d)", p2_pid);

    shmdt(shm_ptr);
}
