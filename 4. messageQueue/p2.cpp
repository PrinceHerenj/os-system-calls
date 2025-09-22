#include <iostream>
#include <sys/_types/_key_t.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/msg.h>

#define MSG_SIZE 100
#define KEY 0x1234

struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    // key_t key = ftok("./mqfile", 65);

    int msgid = msgget(KEY, 0666);

    msg_buffer message;
    for (int i = 0; i < 5; i++) {
        msgrcv(msgid, &message, sizeof(message.msg_text), 2, 0);
        printf("%s\n", message.msg_text);
    }

    return 0;
}
