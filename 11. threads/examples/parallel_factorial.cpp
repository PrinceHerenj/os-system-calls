#include <cstdio>
#include <pthread.h>

#define NUM_THREADS 4
#define MAX_N 20

typedef struct {
    int start, end;
    unsigned long long result;
} ThreadData;

void *partial_factorial(void * arg) {
    ThreadData * data = (ThreadData *) arg;
    data->result = 1;
    for (int i = data->start; i <= data->end; i++) {
        data->result *= i;
    }
    pthread_exit(NULL);
}

int main() {
    int N;
    printf("Enter number <= %d: ", MAX_N);
    scanf("%d", &N);

    if (N <= 0 || N > MAX_N) {
        printf("Invalid Input\n");
        return 1;
    }

    pthread_t threads[NUM_THREADS];
    ThreadData tdata[NUM_THREADS];

    int chunk = N / NUM_THREADS;
    int remainder = N % NUM_THREADS;
    int current = 1;

    for (int i = 0; i < NUM_THREADS; i++) {
        tdata[i].start = current;
        tdata[i].end = current + chunk - 1;
        if (i == NUM_THREADS - 1) tdata[i].end += remainder;
        current = tdata[i].end + 1;
        pthread_create(&threads[i], NULL, partial_factorial, &tdata[i]);
    }

    unsigned long long factorial = 1;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        factorial *= tdata[i].result;
    }

    printf("Factorial of %d is %llu\n", N, factorial);
    return 0;
}
