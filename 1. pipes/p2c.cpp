#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
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
        // parent
        close(pfd[0]);
        write(pfd[1], "Writeas", 7);
        wait(NULL);
    } else {
        // child
        char buf[100];
        close(pfd[1]);
        read(pfd[0], buf, 7);
        cout << buf << endl;
    }

}
