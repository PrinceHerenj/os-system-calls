#include <cstddef>
#include <cstdio>
#include <sys/_types/_key_t.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SIZE_SHM 1024

int main() {
    key_t key = ftok("./shmfile", 65);
    int shmid = shmget(key, SIZE_SHM, 0666);

    void * mem = shmat(shmid, NULL, 0);
    printf("%s", (char *)mem);
    shmdt(mem);

    shmctl(shmid, IPC_RMID, NULL);
}
