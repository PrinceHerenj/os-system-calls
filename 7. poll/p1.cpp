#include <_stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

#define NUM 1
#define SIZE 1024

int main() {
    FILE * streams[NUM];
    pollfd pfds[NUM + 1];
    char buf[SIZE];

    const char *executables[NUM] = {
        "./p2"
    };

    for (int i = 0 ; i < NUM; i++) {
        streams[i] = popen(executables[i], "r");

        int fd = fileno(streams[i]);
        pfds[i].fd = fd;
        pfds[i].events = POLLIN;
    }

    pfds[NUM].fd = STDIN_FILENO;
    pfds[NUM].events = POLLIN;

    int rem = NUM + 1;

    while (rem > 0) {
        int ret = poll(pfds, NUM + 1, -1);

        for (int i = 0; i < NUM; i++) {
            if (pfds[i].fd == -1) continue;

            if (pfds[i].revents & POLLIN) {
                if (fgets(buf, SIZE, streams[i]) != NULL)
                    printf("P1 received from P%d: %s", i+2, buf);
                else {
                    pclose(streams[i]);
                    pfds[i].fd = -1;
                    rem--;
                }
            }
        }

        if (pfds[NUM].revents & POLLIN) {
            if (fgets(buf, SIZE, stdin) != NULL) {
                printf("P1 received from keyboard: %s", buf);
                rem--;
            } else {
                printf("Keyboard input closed. Exiting \n");
                break;
            }
        }
    }

    for (int i = 0; i < NUM; i++) {
        if (pfds[i].fd != -1) {
            pclose(streams[i]);
        }
    }

}
