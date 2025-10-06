#include <cstddef>
#include <cstdio>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 4

int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];

typedef struct {
    int row;
} ThreadData;

void* multiply_row(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int row = data->row;
    for (int col = 0; col < SIZE; col++) {
        C[row][col] = 0;
        for (int k = 0; k < SIZE; k++) {
            C[row][col] += A[row][k] * B[k][col];
        }
    }
    pthread_exit(NULL);
}

int main() {
    printf("Matrix A:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            A[i][j] = i + j;
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }

    printf("Matrix B:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            B[i][j] = i + j;
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }

    pthread_t threads[SIZE];
    ThreadData tdata[SIZE];

    for (int i = 0; i < SIZE; i++) {
        tdata[i].row = i;
        pthread_create(&threads[i], NULL, multiply_row, &tdata[i]);
    }

    for (int i = 0; i < SIZE; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Result Matrix C (A * B):\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            A[i][j] = i + j;
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}
