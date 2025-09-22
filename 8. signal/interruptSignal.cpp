#include <csignal>
#include <cstddef>
#include <cstdio>
#include <sys/_types/_pid_t.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void hdfn(int signo) {
    static int i = 0;
    if (i < 5) {
        printf("Interrupt Cap\n");
    } else {
        signal(SIGUSR1, SIG_DFL);
    }
    i++;
}

int main() {
    signal(SIGUSR1, hdfn);
    for (int i = 0; i < 10; i++) {
        raise(SIGUSR1);
    }
}
