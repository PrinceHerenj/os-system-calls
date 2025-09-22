#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <sys/fcntl.h>

int main() {
    int infd = open("./inputFile.txt", O_RDONLY);
    int outfd = open("./outputFile.txt", O_WRONLY | O_CREAT | O_TRUNC);

    dup2(infd, STDIN_FILENO);
    dup2(outfd, STDOUT_FILENO);

    char buf[100];
    fgets(buf, 15, stdin);
    printf("%s", buf);
}
