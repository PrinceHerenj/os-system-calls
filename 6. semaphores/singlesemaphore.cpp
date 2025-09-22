#include <iostream>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>

using namespace std;

// Semaphore operations
#define P(s) semop(s, &pop, 1)  // Wait (decrement)
#define V(s) semop(s, &vop, 1)  // Signal (increment)

int main() {
    int semid;
    sembuf pop, vop;

    // Create a single semaphore
    semid = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
    if (semid == -1) {
        perror("semget failed");
        return 1;
    }

    // Initialize semaphore to 0 (blocked state)
    semctl(semid, 0, SETVAL, 0);

    // Setup semaphore operations
    pop.sem_num = vop.sem_num = 0;     // Operate on semaphore 0
    pop.sem_flg = vop.sem_flg = 0;     // No special flags
    pop.sem_op = -1;                   // P operation: decrement
    vop.sem_op = 1;                    // V operation: increment

    cout << "=== Simple Semaphore Synchronization Demo ===" << endl;
    cout << "Parent will signal child after 3 seconds..." << endl << endl;

    if (fork() == 0) {
        // CHILD PROCESS
        cout << "[CHILD]  Started! Waiting for parent's signal..." << endl;

        // Wait for parent to signal (this will block until semaphore > 0)
        P(semid);

        cout << "[CHILD]  🎉 Received signal from parent!" << endl;
        cout << "[CHILD]  Now I can do my work!" << endl;

        // Simulate some work
        for (int i = 1; i <= 3; i++) {
            cout << "[CHILD]  Working... step " << i << "/3" << endl;
            sleep(1);
        }

        cout << "[CHILD]  ✅ Work completed! Exiting." << endl;

    } else {
        // PARENT PROCESS
        cout << "[PARENT] Started! Child is waiting..." << endl;
        cout << "[PARENT] I'll do some preparation work first..." << endl;

        // Simulate preparation work
        for (int i = 1; i <= 3; i++) {
            cout << "[PARENT] Preparing... " << i << "/3" << endl;
            sleep(1);
        }

        cout << "[PARENT] 🚀 Preparation done! Signaling child..." << endl;

        // Signal the child (increment semaphore from 0 to 1)
        V(semid);

        cout << "[PARENT] Signal sent! Waiting for child to finish..." << endl;

        // Wait for child to complete
        wait(NULL);

        cout << "[PARENT] ✅ Child finished! Cleaning up..." << endl;

        // Remove semaphore
        semctl(semid, 0, IPC_RMID, 0);

        cout << "[PARENT] 🏁 Program completed successfully!" << endl;
    }

    return 0;
}

/*
COMPILATION:
g++ -o simple_sync simple_sync.cpp

SAMPLE OUTPUT:
=== Simple Semaphore Synchronization Demo ===
Parent will signal child after 3 seconds...

[PARENT] Started! Child is waiting...
[CHILD]  Started! Waiting for parent's signal...
[PARENT] I'll do some preparation work first...
[PARENT] Preparing... 1/3
[PARENT] Preparing... 2/3
[PARENT] Preparing... 3/3
[PARENT] 🚀 Preparation done! Signaling child...
[PARENT] Signal sent! Waiting for child to finish...
[CHILD]  🎉 Received signal from parent!
[CHILD]  Now I can do my work!
[CHILD]  Working... step 1/3
[CHILD]  Working... step 2/3
[CHILD]  Working... step 3/3
[CHILD]  ✅ Work completed! Exiting.
[PARENT] ✅ Child finished! Cleaning up...
[PARENT] 🏁 Program completed successfully!
*/
