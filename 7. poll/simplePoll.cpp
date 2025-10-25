#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <sys/_types/_pid_t.h>
#include <sys/fcntl.h>
#include <sys/ipc.h>
#include <sys/poll.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <vector>
using namespace std;

#define NUM_PIPES 3
#define SHM_KEY 12345
#define MSG_KEY 54321
#define FIFO_PATH "./fifo"
#define BUFFER_SIZE 256

struct msg_buffer {
    long msg_type;
    char msg_text[BUFFER_SIZE];
};

struct shared_data {
    int counter;
    char message[BUFFER_SIZE];
    bool updated;
};

int pipe_fds[NUM_PIPES][2];
int fifo_fd;
int msg_queue_id;
int shmid;
shared_data* shm_ptr;
bool running;
static volatile sig_atomic_t signal_received;

static void signalHandler(int sig) {
    signal_received = sig;
    cout << "\n[SIGNAL] Received signal: " << sig << endl;
}

void checkMessageQueue() {
    if (msg_queue_id == -1) return;

    msg_buffer msg;
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.msg_text), 0, IPC_NOWAIT) != -1) {
        cout << "[MSGQ] Type " << msg.msg_type << ": " << msg.msg_text << endl;
    }
}

void checkSharedMemory() {
    if (shm_ptr && shm_ptr->updated) {
        cout << "[SHM] Counter: " << shm_ptr->counter << ", Message: " << shm_ptr->message << endl;
        shm_ptr->updated = false;
    }
}

void printStatus() {
    cout << "IPC Status" << endl;
    cout << "Pipes created: " << NUM_PIPES << endl;
    cout << "FIFO path: " << FIFO_PATH << endl;
    cout << "Message queue ID: " << msg_queue_id << endl;
    cout << "Shared memory ID: " << shmid << endl;
    if (shm_ptr) {
        cout << "Shared memory counter: " << shm_ptr->counter << endl;
    }
    cout << "Signal received: " << signal_received << endl;
}

void handleUserInput(const char* input) {
    if (strncmp(input, "quit", 4) == 0) {
        cout << "[USER] Quitting.." << endl;
        running = false;
    } else if (strncmp(input, "status", 6) == 0) {
        printStatus();
    } else if (strncmp(input, "signal", 6)) {
        cout << "[USER] Sending SIGUSR1 to self " << endl;
        kill(getpid(), SIGUSR1);
    } else {
        cout << "[USER] Unknown command: " << input;
    }
}

void cleanup() {
    cout << "[CLEANUP] Cleaning up IPC resources" << endl;

    for (int i = 0; i < NUM_PIPES; i++) {
        if (pipe_fds[i][0] != -1) close(pipe_fds[i][0]);
        if (pipe_fds[i][1] != -1) close(pipe_fds[i][1]);
    }

    if (fifo_fd != -1) close(fifo_fd);
    unlink(FIFO_PATH);

    if (shm_ptr) {
        shmdt(shm_ptr);
        shmctl(shmid, IPC_RMID, nullptr);
    }

    if (msg_queue_id != -1) {
        msgctl(msg_queue_id, IPC_RMID, nullptr);
    }
}

static void pipeProducer(int pipe_id, int write_fd) {
    for (int i = 0; i< 5; i++) {
        char msg[BUFFER_SIZE];
        snprintf(msg, BUFFER_SIZE, "Pipe %d message %d\n", pipe_id, i + 1);
        write(write_fd, msg, strlen(msg));
        this_thread::sleep_for(chrono::seconds(3));
    }
    close(write_fd);
    cout << "[PRODUCER] Pipe " << pipe_id << " producer finished" << endl;
}

static void fifoProducer() {
    this_thread::sleep_for(chrono::seconds(1));

    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        perror("FIFO producer: open");
        return;
    }

    for (int i = 0; i < 4; i++) {
        char msg[BUFFER_SIZE];
        snprintf(msg, BUFFER_SIZE, "FIFO message %d\n", i + 1);
        write(fd, msg, strlen(msg));
        this_thread::sleep_for(chrono::seconds(4));
    }

    close(fd);
    cout << "[PRODUCER] FIFO producer finished" << endl;
}

static void messageQueueProducer() {
    this_thread::sleep_for(chrono::seconds(2));

    int mq_id = msgget(MSG_KEY, 0666);
    if (mq_id == -1) {
        perror("MQ producer: msgget");
        return;
    }

    for (int i = 0; i < 3; i++) {
        msg_buffer msg;
        msg.msg_type = i + 1;
        snprintf(msg.msg_text, BUFFER_SIZE, "Message queue message %d", i + 1);
        msgsnd(mq_id, &msg, strlen(msg.msg_text) + 1, 0);
        this_thread::sleep_for(chrono::seconds(5));
    }

    cout << "[PRODUCER] Message queue producer finished" << endl;
}

static void sharedMemoryProducer() {
    this_thread::sleep_for(chrono::seconds(3));

    int shmid = shmget(SHM_KEY, sizeof(shared_data), 0666);
    if (shmid == -1) {
        perror("SHM producer: shmget");
        return;
    }

    shared_data* shm_ptr = (shared_data*) shmat(shmid, nullptr, 0);
    if (shm_ptr == (shared_data*)-1) {
        perror("SHM producer: shmat");
        return;
    }

    for (int i = 0; i < 3; i++) {
        shm_ptr->counter++;
        snprintf(shm_ptr->message, BUFFER_SIZE, "Shared memory update%d", i + 1);
        shm_ptr->updated = true;
        this_thread::sleep_for(chrono::seconds(6));
    }

    shmdt(shm_ptr);
    cout << "[PRODUCER] Shared memory producer finished" << endl;
}

void waitForChildren() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        cout << "[MAIN] Child process " << pid << " exited" << endl;
    }
}

void setupSignalHandler() {
    signal(SIGINT, signalHandler);
    signal(SIGUSR1, signalHandler);
    signal(SIGTERM, signalHandler);
}

void initializeIPC() {
    for (int i = 0; i < NUM_PIPES; i++) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("pipe");
        } else {
            cout << "[INIT] Created PIPE " << i << " (read: " << pipe_fds[i][0] <<", write: " << pipe_fds[i][1] << ")" << endl;
        }
    }

    unlink(FIFO_PATH);
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("mkfifo");
    } else {
        cout << "[INIT] Created FIFO: " << FIFO_PATH << endl;
    }

    msg_queue_id = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msg_queue_id == -1) {
        perror("msgget");
    } else {
        cout << "[INIT] Created message queue ID: " << msg_queue_id << endl;
    }

    shmid = shmget(SHM_KEY, sizeof(shared_data), IPC_CREAT | 0666);
    if (shmid == -1) {
        shm_ptr = (shared_data*)shmat(shmid, nullptr, 0);
        if (shm_ptr != (shared_data*)-1) {
            shm_ptr->counter = 0;
            shm_ptr->updated = false;
            strcpy(shm_ptr->message, "Initial shared memory data");
            cout << "[INIT] Created shared memory" << endl;
        } else {
            perror("shmat");
            shm_ptr = nullptr;
        }
    } else {
        perror("shmget");
    }
}

void spawnProducers() {
    cout << "[SPAWN] Creating producer processes... " << endl;

    for (int i = 0; i < NUM_PIPES; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            close(pipe_fds[i][0]);
            pipeProducer(i, pipe_fds[i][1]);
            exit(0);
        } else if (pid > 0) {
            close(pipe_fds[i][1]);
            cout << "[SPAWN] Pipe producer " << i << " (PID: " << pid << ")" << endl;
        }
    }

    pid_t fifo_pid = fork();
    if (fifo_pid == 0) {
        fifoProducer();
        exit(0);
    } else if (fifo_pid > 0) {
        cout << "[SPAWN] FIFO prodicer (PID: " << fifo_pid << ")" << endl;
    }

    pid_t mq_pid = fork();
    if (mq_pid == 0) {
        messageQueueProducer();
        exit(0);
    } else if (mq_pid > 0) {
        cout << "[SPAWN] Message queue producer (PID: " << fifo_pid << ")" << endl;
    }

    pid_t shm_pid = fork();
    if (shm_pid == 0) {
        sharedMemoryProducer();
        exit(0);
    } else if (shm_pid > 0) {
        cout << "[SPAWN] Shared memory producer (PID: " << shm_pid << ")" << endl;
    }
}

void runConsumer() {
    cout << "[CONSUMER] Starting main consumer with poll().." << endl;

    fifo_fd = open(FIFO_PATH, O_RDONLY | O_NONBLOCK);
    if (fifo_fd == -1) {
        perror("open FIFO");
    }

    vector<pollfd> poll_fds;

    poll_fds.push_back({STDIN_FILENO, POLLIN, 0});

    for (int i = 0; i < NUM_PIPES; i++) {
        if (pipe_fds[i][0] != -1) {
            poll_fds.push_back({pipe_fds[i][0], POLLIN, 0});
        }
    }

    if (fifo_fd != -1) {
        poll_fds.push_back({fifo_fd, POLLIN, 0});
    }

    char buffer[BUFFER_SIZE];
    int consecutive_timeouts = 0;

    cout << "[CONSUMER] Monitoring " << poll_fds.size() << " file descriptors with poll()" << endl;
    cout << "[CONSUMER] Commands: 'status', 'quit', 'signal'" << endl;

    while (running && !signal_received && consecutive_timeouts < 10) {
        int poll_result = poll(poll_fds.data(), poll_fds.size(), 2000);

        if (poll_result == -1) {
            if (errno == EINTR) {
                cout << "[CONSUMER] Poll interrupted by signal" << endl;
                continue;
            }
            perror("poll");
            break;
        }

        if (poll_result == 0) {
            cout << "[CONSUMER] Poll timeout - checking other IPC" << endl;
            checkMessageQueue();
            checkSharedMemory();
            consecutive_timeouts++;
            continue;
        }

        consecutive_timeouts = 0;

        for (size_t i = 0; i < poll_fds.size(); i++) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == STDIN_FILENO) {
                    if (fgets(buffer, BUFFER_SIZE, stdin)) {
                        handleUserInput(buffer);
                    }
                } else if (poll_fds[i].fd == fifo_fd) {
                    ssize_t bytes = read(fifo_fd, buffer, BUFFER_SIZE - 1);
                    if (bytes > 0) {
                        buffer[bytes] = '\0';
                        cout << "[FIFO] Received: " << buffer << endl;
                    }
                } else {
                    ssize_t bytes = read(poll_fds[i].fd, buffer, BUFFER_SIZE - 1);
                    if (bytes > 0) {
                        buffer[bytes] = '\0';
                        cout << "[PIPE] FD " << poll_fds[i].fd << " received: " << buffer << endl;
                    } else if (bytes == 0) {
                        cout << "[PIPE] FD " << poll_fds[i].fd << " closed by producer " << endl;
                       poll_fds[i].fd = -1;
                    }
                }
            }

            if (poll_fds[i].revents & (POLLHUP | POLLERR)) {
                cout << "[POLL] FD " << poll_fds[i].fd << " has error or hangup" << endl;
            }
        }

        checkMessageQueue();
        checkSharedMemory();
    }

    if (consecutive_timeouts >= 10) {
        cout << "[CONSUMER] Too many timeouts, shutting down.." << endl;
    }
}



int main() {
    running = true;
    signal_received = 0;
    initializeIPC();
    setupSignalHandler();

    spawnProducers();
    cout << "[MAIN] All producers spawned. Starting consumer" << endl;
    cout << "[MAIN] PID: " << getpid() << endl;
    cout << "[MAIN] Try: kill -USR1 " << getpid() << " from another terminal" << endl;
    cout << endl;

    runConsumer();
    waitForChildren();

    cleanup();
    cout << "[MAIN] Program completed successfully" << endl;
}
