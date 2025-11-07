#include <cstdio>

#include <unistd.h>
#include <sys/fcntl.h>

int main() {
    int infd = open("./inputFile.txt", O_RDONLY);
    if (infd == -1) {
        perror("Error opening input file");
        return 1;
    }

    int outfd = open("./outputFile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (outfd == -1) {
        perror("Error opening output file");
        close(infd);
        return 1;
    }

    dup2(infd, STDIN_FILENO);
    dup2(outfd, STDOUT_FILENO);

    char buf[100];
    fgets(buf, sizeof(buf), stdin);
    printf("%s", buf);
    close(outfd);
    close(infd);
}
