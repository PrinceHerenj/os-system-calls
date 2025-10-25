#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <signal.h>
#include <fcntl.h>
#include <vector>
#include <errno.h>
#include <thread>
#include <chrono>

// Constants
#define MAX_FDS 10
#define BUFFER_SIZE 256
#define FIFO_PATH "/tmp/demo_fifo"
#define MSG_KEY 12345
#define SHM_KEY 54321
#define NUM_PIPES 3

// Message structure for message queue
struct msg_buffer {
    long msg_type;
    char msg_text[BUFFER_SIZE];
};

// Shared memory structure
struct shared_data {
    int counter;
    char message[BUFFER_SIZE];
    bool updated;
};

class AdvancedIPCDemo {
private:
    int pipe_fds[NUM_PIPES][2];  // Multiple anonymous pipes
    int fifo_fd;                 // Named pipe
    int msg_queue_id;            // Message queue
    int shm_id;                  // Shared memory
    shared_data* shm_ptr;        // Shared memory pointer
    bool running;
    static volatile sig_atomic_t signal_received;

public:
    AdvancedIPCDemo() : running(true), fifo_fd(-1), msg_queue_id(-1), 
                        shm_id(-1), shm_ptr(nullptr) {
        signal_received = 0;
        initializeIPC();
        setupSignalHandler();
    }

    ~AdvancedIPCDemo() {
        cleanup();
    }

    static void signalHandler(int sig) {
        signal_received = sig;
        std::cout << "\n[SIGNAL] Received signal: " << sig << std::endl;
    }

    void setupSignalHandler() {
        signal(SIGINT, signalHandler);
        signal(SIGUSR1, signalHandler);
        signal(SIGTERM, signalHandler);
    }

    void initializeIPC() {
        std::cout << "[INIT] Initializing IPC mechanisms..." << std::endl;

        // Create anonymous pipes
        for (int i = 0; i < NUM_PIPES; i++) {
            if (pipe(pipe_fds[i]) == -1) {
                perror("pipe");
            } else {
                std::cout << "[INIT] Created pipe " << i 
                         << " (read: " << pipe_fds[i][0] 
                         << ", write: " << pipe_fds[i][1] << ")" << std::endl;
            }
        }

        // Create named pipe (FIFO)
        unlink(FIFO_PATH); // Remove if exists
        if (mkfifo(FIFO_PATH, 0666) == -1) {
            perror("mkfifo");
        } else {
            std::cout << "[INIT] Created FIFO: " << FIFO_PATH << std::endl;
        }

        // Create message queue
        msg_queue_id = msgget(MSG_KEY, IPC_CREAT | 0666);
        if (msg_queue_id == -1) {
            perror("msgget");
        } else {
            std::cout << "[INIT] Created message queue ID: " << msg_queue_id << std::endl;
        }

        // Create shared memory
        shm_id = shmget(SHM_KEY, sizeof(shared_data), IPC_CREAT | 0666);
        if (shm_id != -1) {
            shm_ptr = (shared_data*)shmat(shm_id, nullptr, 0);
            if (shm_ptr != (shared_data*)-1) {
                shm_ptr->counter = 0;
                shm_ptr->updated = false;
                strcpy(shm_ptr->message, "Initial shared memory data");
                std::cout << "[INIT] Created shared memory" << std::endl;
            } else {
                perror("shmat");
                shm_ptr = nullptr;
            }
        } else {
            perror("shmget");
        }
    }

    void spawnProducers() {
        std::cout << "[SPAWN] Creating producer processes..." << std::endl;

        // Pipe producers
        for (int i = 0; i < NUM_PIPES; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                // Child process - pipe producer
                close(pipe_fds[i][0]); // Close read end
                pipeProducer(i, pipe_fds[i][1]);
                exit(0);
            } else if (pid > 0) {
                close(pipe_fds[i][1]); // Parent closes write end
                std::cout << "[SPAWN] Pipe producer " << i << " (PID: " << pid << ")" << std::endl;
            }
        }

        // FIFO producer
        pid_t fifo_pid = fork();
        if (fifo_pid == 0) {
            fifoProducer();
            exit(0);
        } else if (fifo_pid > 0) {
            std::cout << "[SPAWN] FIFO producer (PID: " << fifo_pid << ")" << std::endl;
        }

        // Message queue producer
        pid_t mq_pid = fork();
        if (mq_pid == 0) {
            messageQueueProducer();
            exit(0);
        } else if (mq_pid > 0) {
            std::cout << "[SPAWN] Message queue producer (PID: " << mq_pid << ")" << std::endl;
        }

        // Shared memory producer
        pid_t shm_pid = fork();
        if (shm_pid == 0) {
            sharedMemoryProducer();
            exit(0);
        } else if (shm_pid > 0) {
            std::cout << "[SPAWN] Shared memory producer (PID: " << shm_pid << ")" << std::endl;
        }
    }

    void runConsumer() {
        std::cout << "[CONSUMER] Starting main consumer with poll()..." << std::endl;
        
        // Open FIFO for reading (non-blocking)
        fifo_fd = open(FIFO_PATH, O_RDONLY | O_NONBLOCK);
        if (fifo_fd == -1) {
            perror("open FIFO");
        }

        // Setup poll file descriptors
        std::vector<pollfd> poll_fds;
        
        // Add stdin
        poll_fds.push_back({STDIN_FILENO, POLLIN, 0});
        
        // Add pipe read ends
        for (int i = 0; i < NUM_PIPES; i++) {
            if (pipe_fds[i][0] != -1) {
                poll_fds.push_back({pipe_fds[i][0], POLLIN, 0});
            }
        }
        
        // Add FIFO
        if (fifo_fd != -1) {
            poll_fds.push_back({fifo_fd, POLLIN, 0});
        }

        char buffer[BUFFER_SIZE];
        int consecutive_timeouts = 0;
        
        std::cout << "[CONSUMER] Monitoring " << poll_fds.size() 
                 << " file descriptors with poll()" << std::endl;
        std::cout << "[CONSUMER] Commands: 'status', 'quit', 'signal'" << std::endl;

        while (running && !signal_received && consecutive_timeouts < 10) {
            // Poll with 2 second timeout
            int poll_result = poll(poll_fds.data(), poll_fds.size(), 2000);
            
            if (poll_result == -1) {
                if (errno == EINTR) {
                    std::cout << "[CONSUMER] Poll interrupted by signal" << std::endl;
                    continue;
                }
                perror("poll");
                break;
            }
            
            if (poll_result == 0) {
                // Timeout - check other IPC mechanisms
                std::cout << "[CONSUMER] Poll timeout - checking other IPC..." << std::endl;
                checkMessageQueue();
                checkSharedMemory();
                consecutive_timeouts++;
                continue;
            }
            
            consecutive_timeouts = 0; // Reset timeout counter

            // Check which file descriptors are ready
            for (size_t i = 0; i < poll_fds.size(); i++) {
                if (poll_fds[i].revents & POLLIN) {
                    if (poll_fds[i].fd == STDIN_FILENO) {
                        // Handle stdin
                        if (fgets(buffer, BUFFER_SIZE, stdin)) {
                            handleUserInput(buffer);
                        }
                    } else if (poll_fds[i].fd == fifo_fd) {
                        // Handle FIFO input
                        ssize_t bytes = read(fifo_fd, buffer, BUFFER_SIZE - 1);
                        if (bytes > 0) {
                            buffer[bytes] = '\0';
                            std::cout << "[FIFO] Received: " << buffer << std::endl;
                        }
                    } else {
                        // Handle pipe input
                        ssize_t bytes = read(poll_fds[i].fd, buffer, BUFFER_SIZE - 1);
                        if (bytes > 0) {
                            buffer[bytes] = '\0';
                            std::cout << "[PIPE] FD " << poll_fds[i].fd 
                                     << " received: " << buffer << std::endl;
                        } else if (bytes == 0) {
                            std::cout << "[PIPE] FD " << poll_fds[i].fd 
                                     << " closed by producer" << std::endl;
                            poll_fds[i].fd = -1; // Mark as closed
                        }
                    }
                }
                
                if (poll_fds[i].revents & (POLLHUP | POLLERR)) {
                    std::cout << "[POLL] FD " << poll_fds[i].fd 
                             << " has error or hangup" << std::endl;
                }
            }
            
            // Also check other IPC mechanisms
            checkMessageQueue();
            checkSharedMemory();
        }
        
        if (consecutive_timeouts >= 10) {
            std::cout << "[CONSUMER] Too many timeouts, shutting down..." << std::endl;
        }
    }

    void checkMessageQueue() {
        if (msg_queue_id == -1) return;
        
        msg_buffer msg;
        if (msgrcv(msg_queue_id, &msg, sizeof(msg.msg_text), 0, IPC_NOWAIT) != -1) {
            std::cout << "[MSGQ] Type " << msg.msg_type 
                     << ": " << msg.msg_text << std::endl;
        }
    }

    void checkSharedMemory() {
        if (shm_ptr && shm_ptr->updated) {
            std::cout << "[SHM] Counter: " << shm_ptr->counter 
                     << ", Message: " << shm_ptr->message << std::endl;
            shm_ptr->updated = false;
        }
    }

    void handleUserInput(const char* input) {
        if (strncmp(input, "quit", 4) == 0) {
            std::cout << "[USER] Quitting..." << std::endl;
            running = false;
        } else if (strncmp(input, "status", 6) == 0) {
            printStatus();
        } else if (strncmp(input, "signal", 6) == 0) {
            std::cout << "[USER] Sending SIGUSR1 to self..." << std::endl;
            kill(getpid(), SIGUSR1);
        } else {
            std::cout << "[USER] Unknown command: " << input;
        }
    }

    void printStatus() {
        std::cout << "\n=== IPC STATUS ===" << std::endl;
        std::cout << "Pipes created: " << NUM_PIPES << std::endl;
        std::cout << "FIFO path: " << FIFO_PATH << std::endl;
        std::cout << "Message queue ID: " << msg_queue_id << std::endl;
        std::cout << "Shared memory ID: " << shm_id << std::endl;
        if (shm_ptr) {
            std::cout << "Shared memory counter: " << shm_ptr->counter << std::endl;
        }
        std::cout << "Signal received: " << signal_received << std::endl;
        std::cout << "==================\n" << std::endl;
    }

    void cleanup() {
        std::cout << "[CLEANUP] Cleaning up IPC resources..." << std::endl;
        
        // Close pipes
        for (int i = 0; i < NUM_PIPES; i++) {
            if (pipe_fds[i][0] != -1) close(pipe_fds[i][0]);
            if (pipe_fds[i][1] != -1) close(pipe_fds[i][1]);
        }
        
        // Close FIFO
        if (fifo_fd != -1) close(fifo_fd);
        unlink(FIFO_PATH);
        
        // Cleanup shared memory
        if (shm_ptr) {
            shmdt(shm_ptr);
            shmctl(shm_id, IPC_RMID, nullptr);
        }
        
        // Cleanup message queue
        if (msg_queue_id != -1) {
            msgctl(msg_queue_id, IPC_RMID, nullptr);
        }
    }

    // Producer functions (run in child processes)
    static void pipeProducer(int pipe_id, int write_fd) {
        for (int i = 0; i < 5; i++) {
            char msg[BUFFER_SIZE];
            snprintf(msg, BUFFER_SIZE, "Pipe %d message %d\n", pipe_id, i + 1);
            write(write_fd, msg, strlen(msg));
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        close(write_fd);
        std::cout << "[PRODUCER] Pipe " << pipe_id << " producer finished" << std::endl;
    }

    static void fifoProducer() {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for consumer
        
        int fd = open(FIFO_PATH, O_WRONLY);
        if (fd == -1) {
            perror("FIFO producer: open");
            return;
        }
        
        for (int i = 0; i < 4; i++) {
            char msg[BUFFER_SIZE];
            snprintf(msg, BUFFER_SIZE, "FIFO message %d\n", i + 1);
            write(fd, msg, strlen(msg));
            std::this_thread::sleep_for(std::chrono::seconds(4));
        }
        
        close(fd);
        std::cout << "[PRODUCER] FIFO producer finished" << std::endl;
    }

    static void messageQueueProducer() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
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
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        
        std::cout << "[PRODUCER] Message queue producer finished" << std::endl;
    }

    static void sharedMemoryProducer() {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        int shm_id = shmget(SHM_KEY, sizeof(shared_data), 0666);
        if (shm_id == -1) {
            perror("SHM producer: shmget");
            return;
        }
        
        shared_data* shm_ptr = (shared_data*)shmat(shm_id, nullptr, 0);
        if (shm_ptr == (shared_data*)-1) {
            perror("SHM producer: shmat");
            return;
        }
        
        for (int i = 0; i < 3; i++) {
            shm_ptr->counter++;
            snprintf(shm_ptr->message, BUFFER_SIZE, "Shared memory update %d", i + 1);
            shm_ptr->updated = true;
            std::this_thread::sleep_for(std::chrono::seconds(6));
        }
        
        shmdt(shm_ptr);
        std::cout << "[PRODUCER] Shared memory producer finished" << std::endl;
    }

    void waitForChildren() {
        int status;
        pid_t pid;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            std::cout << "[MAIN] Child process " << pid << " exited" << std::endl;
        }
    }
};

// Static member definition
volatile sig_atomic_t AdvancedIPCDemo::signal_received = 0;

int main() {
    std::cout << "=== Advanced IPC with Poll() Demonstration ===" << std::endl;
    std::cout << "This program demonstrates:" << std::endl;
    std::cout << "- poll() system call for I/O multiplexing" << std::endl;
    std::cout << "- Anonymous pipes" << std::endl;
    std::cout << "- Named pipes (FIFOs)" << std::endl;
    std::cout << "- Message queues" << std::endl;
    std::cout << "- Shared memory" << std::endl;
    std::cout << "- Signal handling" << std::endl;
    std::cout << std::endl;

    AdvancedIPCDemo demo;
    
    // Spawn producer processes
    demo.spawnProducers();
    
    std::cout << "[MAIN] All producers spawned. Starting consumer..." << std::endl;
    std::cout << "[MAIN] PID: " << getpid() << std::endl;
    std::cout << "[MAIN] Try: kill -USR1 " << getpid() << " from another terminal" << std::endl;
    std::cout << std::endl;
    
    // Run the main consumer loop
    demo.runConsumer();
    
    // Wait for child processes to finish
    demo.waitForChildren();
    
    std::cout << "[MAIN] Program completed successfully!" << std::endl;
    return 0;
}