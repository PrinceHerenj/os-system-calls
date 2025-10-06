#include <climits>
#include <cstddef>
#include <cstdio>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 16
#define NUM_THREADS 4

int array[ARRAY_SIZE];
int sorted_subarrays[NUM_THREADS][ARRAY_SIZE];

typedef struct {
    int thread_id;
    int start;
    int end;
} ThreadData;

int cmp(const void* a, const void* b) {
    return (*(int*) a - *(int*) b);
}

void* sort_subarray(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    int len = data->end - data->start + 1;

    qsort(&array[data->start], len, sizeof(int), cmp);

    for (int i = 0; i < len; i++) {
        sorted_subarrays[data->thread_id][i] = array[data->start + i];
    }

    pthread_exit(NULL);
}

void merge_sorted(int* result) {
    int indices[NUM_THREADS] = {0};
    int total = ARRAY_SIZE;

    for (int i = 0; i < total; i++) {
        int min_val = INT_MAX;
        int min_idx = -1;

        for (int j = 0; j < NUM_THREADS; j++) {
            int chunk_size = ARRAY_SIZE / NUM_THREADS;
            if (indices[j] < chunk_size) {
                int val = sorted_subarrays[j][indices[j]];
                if (val < min_val) {
                    min_val = val;
                    min_idx = j;
                }
            }
        }

        result[i] = min_val;
        indices[min_idx]++;
    }
}

int main() {
    printf("Original Array: \n");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 100;
        printf("%d ", array[i]);
    }
    printf("\n");

    pthread_t threads[NUM_THREADS];
    ThreadData tdata[NUM_THREADS];
    int chunk_size = ARRAY_SIZE / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        tdata[i].thread_id = i;
        tdata[i].start = i * chunk_size;
        tdata[i].end = (i + 1) * chunk_size - 1;
        pthread_create(&threads[i], NULL, sort_subarray, &tdata[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    int final_sorted[ARRAY_SIZE];
    merge_sorted(final_sorted);
    printf("Final Sorted Array: \n");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", final_sorted[i]);
    }
    printf("\n");
    return 0;
}
