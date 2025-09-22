#include <cstdio>
#include <sys/_types/_key_t.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/shm.h>

#define SHM_SIZE 1024

int main() {
    key_t key = ftok("./shmfile", 65);
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);

    void *mem = shmat(shmid, NULL, 0);

    snprintf((char *)mem, SHM_SIZE, "Hello from writer %d", getpid());
    shmdt(mem);
}
