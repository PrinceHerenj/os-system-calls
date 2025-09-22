#include <iostream>
#include <pthread.h>
using namespace std;

char* st = "Child thread";
void* fun(void *st) {
    cout << "Created child thread: " << (char*)st;
}

int main() {
    pthread_t t;
    pthread_create(&t, NULL, &fun, (void*)st);
    cout << "Created Main thread!" << endl;
    pthread_join(t, NULL);
    exit(EXIT_SUCCESS);
    return 0;
}
