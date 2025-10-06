#include <cstddef>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/_pthread/_pthread_t.h>

#define NUM_THREADS 4

typedef struct {
    int start;
    int end;
    int thread_id;
} ThreadData;

int is_prime(int num) {
    if (num <= 1) return 0;
    if (num == 2) return 1;
    if (num % 2 == 0) return 0;

    int sqrt_n = (int) sqrt(num);
    for(int i = 3; i <= sqrt_n; i += 2) {
        if (num % i == 0) return 0;
    }
    return 1;
}

void* find_primes(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    printf("Thread %d: Calculating primes between %d and %d\n", data->thread_id, data->start, data->end);
    for (int i = data->start; i <= data->end; i++) {
        if (is_prime(i)) {
            printf("Thread %d: %d\n", data->thread_id, i);
        }
    }

    pthread_exit(NULL);
}

int main() {
    int N;
    printf("Enter the maximum number (must be divisible by 4): ");
    scanf("%d", &N);

    if(N % NUM_THREADS != 0 || N <= 0) {
        printf("Error: Number must be positive and divisible by 4.\n");
        return 1;
    }

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    int range = N / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start = i * range + 1;
        thread_data[i].end = (i + 1) * range;
        thread_data[i].thread_id = i;
        pthread_create(&threads[i], NULL, find_primes, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads completed\n");
    return 0;
}
