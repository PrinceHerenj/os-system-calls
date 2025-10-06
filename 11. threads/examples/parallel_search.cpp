#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>

#define ARRAY_SIZE 1000
#define NUM_THREADS 4

int array[ARRAY_SIZE];
int target;
int found = 0;
pthread_mutex_t found_lock;

typedef struct {
    int start, end;
} ThreadData;

void * search(void * arg) {
    ThreadData * data = (ThreadData*) arg;
    for (int i = data->start; i <= data->end && !found; i++) {
        if (array[i] == target) {
            pthread_mutex_lock(&found_lock);
            found = 1;
            printf("Found target %d at index %d (Thread %d - %d)\n",
                target, i, data->start, data->end);
            pthread_mutex_unlock(&found_lock);
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}

int main() {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 10000;
    }

    for (auto x: array) {
        printf("%d ", x);
    }

    printf("Enter a number to search: ");
    scanf("%d", &target);

    pthread_t threads[NUM_THREADS];
    ThreadData tdata[NUM_THREADS];
    int chunk = ARRAY_SIZE / NUM_THREADS;

    pthread_mutex_init(&found_lock, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        tdata[i].start = i * chunk;
        tdata[i].end = (i == NUM_THREADS - 1) ? (ARRAY_SIZE - 1): ((i + 1) * chunk - 1);
        pthread_create(&threads[i], NULL, search, &tdata[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    if (!found) {
        printf("Target not found in array. \n");
    }

    pthread_mutex_destroy(&found_lock);
    return 0;
}
