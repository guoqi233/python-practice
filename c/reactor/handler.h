#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

class ClientHandler
{
private:
    char buffer[1024];
    unsigned int recvd_len;

public:
    int client_fd;

public:
    int do_recv();
    void do_close();
    void init(int fd);
    ~ClientHandler();
};