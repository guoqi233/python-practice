#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int oldSocketFlag = fcntl(listen_fd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    fcntl(listen_fd, F_SETFL, newSocketFlag);
}