#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
using namespace std;

string str;

void *fun (void *) {
    sleep(1);
    cout << "Created child thread" << str << endl;
}

int main() {
    pthread_t t[4];
    cout << "CSE NITW Multithreading!" << endl << endl;
    for (int i = 0; i < 4; i++) {
        cout << "Created -> Thread T[" << i << "]" << str << endl;

        pthread_create(&t[i], NULL, &fun, NULL);
        pthread_join(t[i], NULL);
    }
    exit(EXIT_SUCCESS);
    return 0;
}
