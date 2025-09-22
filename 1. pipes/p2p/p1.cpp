#include <cstddef>
#include <cstdio>
#include <iostream>
#include <unistd.h>
using namespace std;

int main() {
    int pfd[2];
    if (pipe(pfd) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid > 0) {
        close(pfd[0]);
        write(pfd[1], "some", 4);
        wait(NULL);
    } else {
        dup2(pfd[0], 0);
        execlp("./p2.out", NULL);
    }
}
