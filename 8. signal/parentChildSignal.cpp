#include <csignal>
#include <stdio.h>
#include <sys/_types/_pid_t.h>
#include <unistd.h>
#include <sys/wait.h>


void hdfn1(int signo) {
    printf("Signal from child\n");
}

void hdfn2(int signo) {
    printf("Signal from parent\n");
}

int main() {
    int pid = getpid();
    pid_t childpid = fork();

    if (childpid > 0) {
        signal(SIGUSR2, hdfn1);
        sleep(1);
        kill(childpid, SIGUSR1);
        wait(NULL);
    } else {
        signal(SIGUSR1, hdfn2);
        sleep(2);
        kill(pid, SIGUSR2);
    }
}
