#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_FDS 10
#define BUFFER_SIZE 256
#define FIFO1_PATH "./fifo1"
#define FIFO2_PATH "./fifo2"

int main() {
    printf("=== Poll() Demonstration - Server ===\n");
    printf("Server PID: %d\n", getpid());

    // Create FIFOs for communication
    mkfifo(FIFO1_PATH, 0666);
    mkfifo(FIFO2_PATH, 0666);

    // Create pipes for child process communication
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(1);
    }

    // Fork a child process that will send data periodically
    pid_t child_pid = fork();
    if (child_pid == 0) {
        // Child process - data generator
        close(pipe1[0]); // Close read end
        close(pipe2[0]);

        for (int i = 1; i <= 5; i++) {
            sleep(2); // Send data every 2 seconds
            char msg[BUFFER_SIZE];
            snprintf(msg, BUFFER_SIZE, "Child message %d from PID %d\n", i, getpid());

            if (i % 2 == 1) {
                write(pipe1[1], msg, strlen(msg));
                printf("[CHILD] Sent to pipe1: %s", msg);
            } else {
                write(pipe2[1], msg, strlen(msg));
                printf("[CHILD] Sent to pipe2: %s", msg);
            }
        }

        close(pipe1[1]);
        close(pipe2[1]);
        exit(0);
    }

    // Parent process - poll server
    close(pipe1[1]); // Close write ends
    close(pipe2[1]);

    // Open FIFOs in non-blocking mode
    int fifo1_fd = open(FIFO1_PATH, O_RDONLY | O_NONBLOCK);
    int fifo2_fd = open(FIFO2_PATH, O_RDONLY | O_NONBLOCK);

    // Set up poll file descriptor array
    struct pollfd pfds[5];
    int num_fds = 5;

    // STDIN
    pfds[0].fd = STDIN_FILENO;
    pfds[0].events = POLLIN;

    // Pipe 1
    pfds[1].fd = pipe1[0];
    pfds[1].events = POLLIN;

    // Pipe 2
    pfds[2].fd = pipe2[0];
    pfds[2].events = POLLIN;

    // FIFO 1
    pfds[3].fd = fifo1_fd;
    pfds[3].events = POLLIN;

    // FIFO 2
    pfds[4].fd = fifo2_fd;
    pfds[4].events = POLLIN;

    char buffer[BUFFER_SIZE];
    int active_fds = num_fds;
    int iterations = 0;

    printf("\n[SERVER] Monitoring multiple file descriptors:\n");
    printf("  - STDIN (fd %d)\n", STDIN_FILENO);
    printf("  - Pipe1 (fd %d)\n", pipe1[0]);
    printf("  - Pipe2 (fd %d)\n", pipe2[0]);
    printf("  - FIFO1 (fd %d)\n", fifo1_fd);
    printf("  - FIFO2 (fd %d)\n", fifo2_fd);
    printf("\nInstructions:\n");
    printf("  - Type messages to send via STDIN\n");
    printf("  - Run clients to send FIFO messages\n");
    printf("  - Type 'quit' to exit\n\n");

    while (active_fds > 0 && iterations < 20) {
        printf("[POLL] Waiting for events... (iteration %d)\n", ++iterations);

        // Poll with 5-second timeout
        int poll_result = poll(pfds, num_fds, 5000);

        if (poll_result == -1) {
            perror("poll");
            break;
        } else if (poll_result == 0) {
            printf("[POLL] Timeout occurred - no events in 5 seconds\n");
            continue;
        }

        printf("[POLL] %d file descriptor(s) ready\n", poll_result);

        // Check each file descriptor
        for (int i = 0; i < num_fds; i++) {
            if (pfds[i].fd == -1) continue; // Skip closed fds

            if (pfds[i].revents & POLLIN) {
                printf("[EVENT] Data available on fd %d (index %d)\n", pfds[i].fd, i);

                ssize_t bytes_read = read(pfds[i].fd, buffer, BUFFER_SIZE - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';

                    switch (i) {
                        case 0: // STDIN
                            printf("[STDIN] Received: %s", buffer);
                            if (strncmp(buffer, "quit", 4) == 0) {
                                printf("[SERVER] Quit command received\n");
                                active_fds = 0;
                            }
                            break;
                        case 1: // Pipe 1
                            printf("[PIPE1] Received: %s", buffer);
                            break;
                        case 2: // Pipe 2
                            printf("[PIPE2] Received: %s", buffer);
                            break;
                        case 3: // FIFO 1
                            printf("[FIFO1] Received: %s", buffer);
                            break;
                        case 4: // FIFO 2
                            printf("[FIFO2] Received: %s", buffer);
                            break;
                    }
                } else if (bytes_read == 0) {
                    printf("[EOF] fd %d closed by peer\n", pfds[i].fd);
                    close(pfds[i].fd);
                    pfds[i].fd = -1;
                    active_fds--;
                } else {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        perror("read");
                    }
                }
            }

            if (pfds[i].revents & POLLHUP) {
                printf("[HANGUP] fd %d hung up\n", pfds[i].fd);
                close(pfds[i].fd);
                pfds[i].fd = -1;
                active_fds--;
            }

            if (pfds[i].revents & POLLERR) {
                printf("[ERROR] Error on fd %d\n", pfds[i].fd);
                close(pfds[i].fd);
                pfds[i].fd = -1;
                active_fds--;
            }
        }

        printf("\n");
    }

    // Cleanup
    wait(NULL); // Wait for child process

    if (fifo1_fd != -1) close(fifo1_fd);
    if (fifo2_fd != -1) close(fifo2_fd);
    if (pipe1[0] != -1) close(pipe1[0]);
    if (pipe2[0] != -1) close(pipe2[0]);

    unlink(FIFO1_PATH);
    unlink(FIFO2_PATH);

    printf("[SERVER] Cleanup completed\n");
    return 0;
}
