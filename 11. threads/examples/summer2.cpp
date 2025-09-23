#include <cstdlib>
#include <pthread.h>
#include <cstdio>
int sum;
void * runner(void *param);

int main(int argc, char * argv[]) {
    pthread_t tid;
    pthread_attr_t attr;

    int limit;
    if (argc != 2) {
        fprintf(stderr, "Usage: ./summer.out value\n");
        return -1;
    }

    limit = atoi(argv[1]);
    if (limit < 0) {
        fprintf(stderr, "Value must be non-negative\n");
        return -1;
    }

    pthread_attr_init(&attr);

    pthread_create(&tid, &attr, runner, argv[1]);
    pthread_join(tid, NULL);

    printf("%d %d", limit, sum);
}

void * runner(void *param) {
    int i;
    int upper;

    upper = atoi((char*)param);
    sum = 0;

    for(i = 1; i <= upper; i++) sum += i;

    pthread_exit(0);
}
