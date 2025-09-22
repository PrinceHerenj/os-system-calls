#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <sys/fcntl.h>

int main() {
    int fd = open("./inputFile.txt", O_RDONLY);
    dup2(fd, STDIN_FILENO);
    char buf[100];
    fgets(buf, 20, stdin);
    printf("File contains: %s", buf);
    return 1;
}
