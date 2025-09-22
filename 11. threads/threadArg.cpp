#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
using namespace std;

#define NUM_THREADS 3

struct data {
    int thread_id;
    char * msg;
};

void *DisplayHi(void *thread_arg) {
    struct data * val;
    val = (struct data *) thread_arg;

    cout << "Thread ID: " << val->thread_id;
    cout << ", Message: " << val->msg << endl;
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    struct data td[NUM_THREADS];

    int rc;
    int i;

    for (int i = 0; i < NUM_THREADS; i++) {
        cout << "Creating Thread: " << i << endl;
        td[i].thread_id = i;
        td[i].msg = "Just a friendly message";
        rc = pthread_create(&threads[i], NULL, DisplayHi, (void*)&td[i]);
        if (rc) {
            cout << "Error: Unable to create thread" << rc << endl;
            exit(0);
        }
    }

    pthread_exit(NULL);
}
