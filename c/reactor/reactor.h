#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <poll.h>
#include <iostream>
#include <vector>
#include <errno.h>
#include "subReactor.h"
#include "handler.h"

#define INVALID_FD -1
typedef struct sockaddr sa;
typedef struct sockaddr_in sa_in;

class Reactor final
{
public:
    int listenfd;
    int epollfd;

public:
    void init();
    void start();

private:
    SubReactor *subreactor;
};
