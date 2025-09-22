#include <stdio.h>
#include <unistd.h>

int main() {
    sleep(2);
    printf("Hello from p2\n");
    fflush(stdout);
    return 0;
}
