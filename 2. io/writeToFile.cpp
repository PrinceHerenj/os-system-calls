#include <iostream>
#include <unistd.h>
#include <sys/fcntl.h>

int main() {
    int fd = open("./outputFile.txt", O_WRONLY | O_CREAT | O_TRUNC);

    dup2(fd, STDOUT_FILENO);
    std::cout << "This will be written to file";
    close(fd);
}
