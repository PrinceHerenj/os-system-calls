#include <cstddef>
#include <cstdio>
#include <stdlib.h>
#include <pthread.h>

#define NUM_READERS 5
#define NUM_WRITERS 2

int data = 0;

pthread_mutex_t data_mutex;
pthread_mutex_t readers_mutex;

int readers_count = 0;

void* reader(void* arg) {
    pthread_mutex_lock(&readers_mutex);
    readers_count++;
    if (readers_count == 1) {
        pthread_mutex_lock(&data_mutex);
    }
    pthread_mutex_unlock(&readers_mutex);

    printf("Reader %ld read data: %d\n", (long)arg, data);

    pthread_mutex_lock(&readers_mutex);
    readers_count--;
    if (readers_count == 0) {
        pthread_mutex_unlock(&data_mutex);
    }
    pthread_mutex_unlock(&readers_mutex);

    pthread_exit(NULL);
}

void *writer(void *arg) {
    pthread_mutex_lock(&data_mutex);
    data++;

    printf("Writer %ld updated data: %d\n", (long)arg, data);
    pthread_mutex_unlock(&data_mutex);

    pthread_exit(NULL);
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];

    pthread_mutex_init(&data_mutex, NULL);
    pthread_mutex_init(&readers_mutex, NULL);

    for (long i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, writer, (void*)i);
    }

    for (long i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader, (void*) i);
    }

    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(writers[i], NULL);
    }

    pthread_mutex_destroy(&data_mutex);
    pthread_mutex_destroy(&readers_mutex);
}
