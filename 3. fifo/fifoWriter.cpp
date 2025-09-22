#include <iostream>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>

#define FIFO_PATH "./myfifo"

int main() {
    char message[] = "Message";
    int fifo_fd;

    mkfifo(FIFO_PATH, 0666);

    fifo_fd = open(FIFO_PATH, O_WRONLY);

    write(fifo_fd, message, sizeof(message));
    close(fifo_fd);
}
