#include <cstdio>
#include <cstdlib>
int main(int argc, char *argv[]) {
    int i, limit, sum;
    if (argc != 2) {
        fprintf(stderr, "Usage: ./summer.out value\n");
        return -1;
    }

    limit = atoi(argv[1]);
    if (limit < 0) {
        fprintf(stderr, "Value must be non-negative\n");
        return -1;
    }

    sum = 0;

    for (i = 1; i <= limit; i++) {
        sum += i;
    }

    printf("%d %d", limit, sum);


}
