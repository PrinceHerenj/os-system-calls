#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include<pthread.h>

int sum;
int checksum;

void *runner(void * param);
void *checker(void * param);

int main(int argc, char *argv[]) {
    pthread_t tid1, tid2;
    pthread_attr_t attr;
    int limit;

    if(argc != 2) {
        fprintf(stderr, "Usage ./summer3 value\n");
        return -1;
    }

    limit = atoi(argv[1]);

    if (limit < 0) {
        fprintf(stderr, "Value must be non negative\n");
        return -1;
    }

    pthread_attr_init(&attr);

    pthread_create(&tid1, &attr, runner, argv[1]);
    pthread_create(&tid2, &attr, checker, argv[1]);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("Obtained: %d %d\n", limit, sum);
}

void *runner(void *param) {
    int i;
    int upper;
    upper = atoi((char*)param);

    for (i = 1; i <= upper; i++) sum += i;

    pthread_exit(0);
}

void *checker(void *param) {
    int answer;
    int n;

    n = atoi((char*)param);

    answer = n * (n + 1) / 2;
    printf("Should be: %d %d\n", n, answer);

    pthread_exit(0);
}
