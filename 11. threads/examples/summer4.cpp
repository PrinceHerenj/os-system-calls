#include <cstdio>
#include <pthread.h>
#include <cstdlib>
using namespace std;

int sum, sumodd, sumeven;
int checksum;

void *runner1(void *param);
void *runner2(void *param);
void *checker(void *param);

int main(int argc, char *argv[]) {
    pthread_t tid1, tid2, tid3;
    pthread_attr_t attr;
    int limit;

    if (argc != 2) {
        fprintf(stderr, "Usage: ./summer4 value\n");
        return -1;
    }

    limit = atoi(argv[1]);

    if (limit < 0) {
        fprintf(stderr, "Value must be non-negative\n");
        return -1;
    }

    pthread_attr_init(&attr);

    pthread_create(&tid1, &attr, runner1, argv[1]);
    pthread_create(&tid2, &attr, runner2, argv[1]);
    pthread_create(&tid3, &attr, checker, argv[1]);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    sum = sumodd + sumeven;

    printf("Obtained: %d %d\n", limit, sum);
}

void *runner1(void *param) {
    int i;
    int upper;

    upper = atoi((char*)param);
    sumodd = 0;
    for (i = 1; i <= upper; i += 2) sumodd += i;
    printf("Odd: %d\n", sumodd);
    pthread_exit(0);
}

void *runner2(void *param) {
    int i;
    int upper;

    upper = atoi((char*)param);
    sumeven = 0;
    for (i = 0; i <= upper; i += 2) sumeven += i;
    printf("Even: %d\n", sumeven);
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
