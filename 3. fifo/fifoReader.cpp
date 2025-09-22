#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <iostream>
using namespace std;

#define FIFO_PATH "./myfifo"

int main() {
    char buf[100];

    mkfifo(FIFO_PATH, 0666);
    int fifo_fd = open(FIFO_PATH, O_RDONLY);
    read(fifo_fd, buf, 100);
    cout << buf;
}
