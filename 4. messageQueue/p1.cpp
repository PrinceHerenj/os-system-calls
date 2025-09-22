#include <cstdio>
#include <iostream>
#include <sys/_types/_key_t.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_SIZE 100
#define KEY 0x1234

struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    // key_t key = ftok("./mqfile", 65);

    int msgid = msgget(KEY, 0666 | IPC_CREAT);

    msg_buffer message;
    message.msg_type = 2;

    for (int i = 0; i < 5; i++) {
        snprintf(message.msg_text, MSG_SIZE, "Message from Process1");
        msgsnd(msgid, &message, sizeof(message.msg_text), 0);
        printf("%s\n", message.msg_text);
    }
}
