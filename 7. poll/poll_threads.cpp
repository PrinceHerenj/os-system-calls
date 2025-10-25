#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <iostream>

void* worker(void* arg) {
    int* pipe_fds = (int*)arg;
    sleep(2);
    char done = '1';
    write(pipe_fds[1], &done, 1);
    return nullptr;
}

int main() {
    int pipe_fds[2];
    pipe(pipe_fds);

    pthread_t thread;
    pthread_create(&thread, nullptr, worker, pipe_fds);

    struct pollfd pfd;
    pfd.fd = pipe_fds[0];
    pfd.events = POLLIN;

    std::cout << "Polling thread...\n";
    int ret = poll(&pfd, 1, 5000); // 5 second timeout

    if (ret > 0 && (pfd.revents & POLLIN)) {
        std::cout << "Thread completed!\n";
    }

    pthread_join(thread, nullptr);
    close(pipe_fds[0]);
    close(pipe_fds[1]);

    return 0;
}
