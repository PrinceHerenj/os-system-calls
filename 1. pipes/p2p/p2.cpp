#include <iostream>
#include <unistd.h>

int main() {
    char buf[100];
    read(0, buf, 4);
    buf[4] = '\0';
    std::cout << buf << std::endl;
}
