#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sys/_types/_ssize_t.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/poll.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/signal.h>
using namespace std;

#define NUM_REPORTERS 3
#define NEWS_PER_REPORTER 3
#define NUM_READERS 2
#define SHM_KEY 1234
#define SEM_KEY 5678
#define MSG_KEY 9876
#define BUFFER_SIZE 256

typedef struct {
    char message[BUFFER_SIZE];
} shared_data;

typedef struct {
    long msg_type;
    char msg_text[BUFFER_SIZE];
} msg_buffer;

int editor_pipe_fd[2];
int reporter_pipe_fds[NUM_REPORTERS][2];
int news_reader_pipe_fds[NUM_READERS][2];
int msg_queue_id;
int advert_shmid;
shared_data* shm_ptr;
int news_reader_semid;
int reporter_ids[NUM_REPORTERS];
int news_reader_ids[NUM_READERS];
int news_item_count = 1;

pthread_t reporter_threads[NUM_REPORTERS];
pthread_t editor_pthread, advert_pthread, screen_pthread;
pthread_t news_reader_pthread[NUM_READERS];

volatile sig_atomic_t advert_signal_received = 0;

void sem_wait(int semid){
    struct sembuf sb = {0, -1, 0};

    if (semop(semid, &sb, 1) == -1) {
        perror("semop wait failed");
        exit(1);
    }
}

void sem_signal(int semid) {
    struct sembuf sb = {0, 1, 0};

    if (semop(semid, &sb, 1) == -1) {
        perror("semop signal failed");
        exit(1);
    }
}

void advert_signal_handler(int signum) {
    if (signum == SIGUSR1) {
        advert_signal_received = 1;
    }
}

void * editor_thread(void * arg) {
    cout << "Editor initialized" << endl;
    sleep(2);

    if (signal(SIGUSR1, advert_signal_handler) == SIG_ERR) {
        perror("signal");
        return NULL;
    }

    struct pollfd pollfds[NUM_REPORTERS];
    for (int i = 0; i < NUM_REPORTERS; i++) {
        pollfds[i].fd = reporter_pipe_fds[i][0];
        pollfds[i].events = POLLIN;
        pollfds[i].revents = 0;
    }

    bool running = true;
    int timeout_count = 0;

    char buffer[BUFFER_SIZE];
    int current_msg_type = 1;
    while(running) {
        if (advert_signal_received) {
            advert_signal_received = 0;

            if (shm_ptr != nullptr) {
                // cout << "[EDITOR] Received advertisement signal, reading from shared memory" << endl;
                int len = strlen(shm_ptr->message);
                write(editor_pipe_fd[1], shm_ptr->message, len);
                write(editor_pipe_fd[1], "\n", 1);
                // cout << "[EDITOR] Forwarded advertisement to screen: " << shm_ptr->message << endl;
            }
        }

        int poll_result = poll(pollfds, NUM_REPORTERS, 3000);
        if (poll_result > 0) {
            for (int i = 0; i < NUM_REPORTERS; i++) {
                if (pollfds[i].revents & POLLIN) {
                    ssize_t bytes_read = read(reporter_pipe_fds[i][0], buffer, BUFFER_SIZE - 1);
                    if (bytes_read > 0) {
                        buffer[bytes_read] = '\0';
                        // cout << "[EDITOR] Received from Reporter" << i + 1 << ": " << buffer;
                        msg_buffer news_item;
                        news_item.msg_type = current_msg_type;
                        strncpy(news_item.msg_text, buffer, BUFFER_SIZE - 1);
                        news_item.msg_text[BUFFER_SIZE - 1] = '\0';
                        msgsnd(msg_queue_id, &news_item, strlen(news_item.msg_text) + 1, 0);
                        current_msg_type = (current_msg_type % NUM_READERS) + 1;
                    }
                }
            }
        } else if (poll_result == 0) {
            // cout << "[EDITOR] Waiting for news." << endl;
            timeout_count++;
            if (timeout_count > 5) {
                cout << "[EDITOR] No activity for a while, poll shutting down" << endl;
                running = false;
            }
        } else {
            if (errno == EINTR) {
                // cout << "[EDITOR] poll() interrupted by signal" << endl;
                continue;
            }
            perror("poll");
            break;
        }
    }

    return NULL;
}

void * reporter_thread(void * arg) {
    int reporter_id = *(int *) arg;
    cout << "Reporter " << reporter_id + 1 << " initialized" << endl;

    sleep(2);
    for (int i = 0; i < NEWS_PER_REPORTER; i++) {
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "News Item %d\n", news_item_count++);
        write(reporter_pipe_fds[reporter_id][1], buffer, strlen(buffer));
        sleep(1);
    }

    return NULL;
}

void * news_reader_thread(void * arg) {
    int news_reader_id = *(int *) arg;
    cout << "News Reader " << news_reader_id + 1 << " initialized" << endl;

    long msg_type = news_reader_id + 1;

    sleep(2);

    while (1) {
        msg_buffer news_item;
        ssize_t msg_size = msgrcv(msg_queue_id, &news_item, BUFFER_SIZE, msg_type, 0);
        if (msg_size > 0) {
            char buffer[BUFFER_SIZE];
            snprintf(buffer, BUFFER_SIZE, "[NEWS READER %d] Read: %s", news_reader_id + 1, news_item.msg_text);
            write(news_reader_pipe_fds[news_reader_id][1], buffer, strlen(buffer));
        } else if(msg_size == -1) {
            perror("msgrcv");
            break;
        }
        sleep(2);
    }
    return NULL;
}

void * screen_thread(void * arg) {
    cout << "Screen initialized" << endl;

    sleep(2);

    struct pollfd pollfds[NUM_READERS + 1];
    for (int i = 0; i < NUM_READERS; i++) {
        pollfds[i].fd = news_reader_pipe_fds[i][0];
        pollfds[i].events = POLLIN;
        pollfds[i].revents = 0;
    }

    // Advert polling
    pollfds[NUM_READERS].fd = editor_pipe_fd[0];
    pollfds[NUM_READERS].events = POLLIN;
    pollfds[NUM_READERS].revents = 0;

    bool running = true;
    int timeout_count = 0;

    char buffer[BUFFER_SIZE];
    while(running) {
        int poll_result = poll(pollfds, NUM_READERS + 1, 3000);
        if (poll_result > 0) {
            for (int i = 0; i < NUM_READERS + 1; i++) {
                if (pollfds[i].revents & POLLIN) {
                        ssize_t bytes_read;

                        if (i < NUM_READERS) {
                            // Read from news reader pipes
                            bytes_read = read(news_reader_pipe_fds[i][0], buffer, BUFFER_SIZE - 1);
                        } else {
                            // Read from editor pipe (advertisements)
                            bytes_read = read(editor_pipe_fd[0], buffer, BUFFER_SIZE - 1);
                        }

                        if (bytes_read > 0) {
                            buffer[bytes_read] = '\0';
                            cout << "[SCREEN] " << buffer;
                        }
                    }
            }
        } else if (poll_result == 0) {
            timeout_count++;
            if (timeout_count > 5) {
                cout << "[SCREEN] No activity for a while, poll shutting down" << endl;
                running = false;
            }
        } else {
            perror("poll");
            break;
        }

    }

    return NULL;
}

void * advertiser_thread(void * arg) {
    cout << "Advertiser initialized" << endl;
    sleep(6);

    for (int i = 0; i < 3; i++) {
        if (shm_ptr != nullptr) {
            char advert_msg[BUFFER_SIZE];
            snprintf(advert_msg, BUFFER_SIZE, "Advertisement %d", i + 1);
            strncpy(shm_ptr->message, advert_msg, BUFFER_SIZE - 1);
            shm_ptr->message[BUFFER_SIZE - 1] = '\0';

            if (pthread_kill(editor_pthread, SIGUSR1) == 0) {
                // cout << "[ADVERTISER] Sent SIGUSR1 to editor thread" << endl;
            } else {
                perror("pthread_kill");
            }
        }
        sleep(2);
    }


    return NULL;
}

void initIPC() {
    // Create Editor Pipes
    if (pipe(editor_pipe_fd) == -1) {
        perror("pipe");
    } else {
        cout << "[INIT] Editor Pipe initialized" << endl;
    }

    // Create reporter Pipes
    for (int i = 0; i < NUM_REPORTERS; i++) {
        if (pipe(reporter_pipe_fds[i]) == -1) {
            perror("pipe");
        } else {
            cout << "[INIT] Reporter Pipe " << i + 1 << " initialized" << endl;
        }
    }

    // Create news reader Pipes
    for (int i = 0; i < NUM_READERS; i++) {
        if (pipe(news_reader_pipe_fds[i]) == -1) {
            perror("pipe");
        } else {
            cout << "[INIT] News Reader Pipe " << i + 1 << " initialized" << endl;

        }
    }

    // Create message queue
    msg_queue_id = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msg_queue_id == -1) {
        perror("msgget");
    } else {
        cout << "[INIT] Created message queue "<< endl;
    }

    advert_shmid = shmget(SHM_KEY, sizeof(shared_data), IPC_CREAT | 0666);
    if (advert_shmid != -1) {
        shm_ptr = (shared_data*)shmat(advert_shmid, nullptr, 0);
        if (shm_ptr != (shared_data*)-1) {
            strcpy(shm_ptr->message, "Initialized");
            cout << "[INIT] Created Shared Memory" << endl;
        } else {
            perror("shmat");
            shm_ptr = nullptr;
        }
    } else {
        perror("shmget");
    }

    news_reader_semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (news_reader_semid == -1) {
        perror("semget");
    } else {
        union semun arg;
        arg.val = 1;
        if (semctl(news_reader_semid, 0, SETVAL, arg) == -1) {
            perror("semctl");
        }
    }
}

void createThreads() {
    // create screen thread
    pthread_create(&screen_pthread, NULL, screen_thread, NULL);

    // create editor thread
    pthread_create(&editor_pthread, NULL, editor_thread, NULL);

    // create reporter threads
    for (int i = 0; i < NUM_REPORTERS; i++) {
        reporter_ids[i] = i;
        pthread_create(&reporter_threads[i], NULL, reporter_thread, &reporter_ids[i]);
    }

    // create news reader threads
    for (int i = 0; i < NUM_READERS; i++) {
        news_reader_ids[i] = i;
        pthread_create(&news_reader_pthread[i], NULL, news_reader_thread, &news_reader_ids[i]);
    }

    // create advert thread
    pthread_create(&advert_pthread, NULL, advertiser_thread, NULL);
}

void cleanup() {
    for (int i = 0; i < NUM_REPORTERS; i++)
        pthread_join(reporter_threads[i], NULL);

    pthread_join(editor_pthread, NULL);

    for (int i = 0; i < NUM_READERS; i++)
        pthread_cancel(news_reader_pthread[i]);

    pthread_join(advert_pthread, NULL);

    pthread_join(screen_pthread, NULL);

    cout << "\nCleaned Up" << endl;
}

int main() {


    initIPC();
    createThreads();
    cleanup();
}
