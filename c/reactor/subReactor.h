#pragma once
#include <map>
#include <list>
#include <thread>
#include <mutex>
#include <memory>
#include <iostream>
#include <sys/epoll.h>
#include <errno.h>
#include <functional>
#include "handler.h"

class SubReactor
{
private:
    std::map<int, std::shared_ptr<ClientHandler>> fd_handler_map;
    std::list<std::shared_ptr<ClientHandler>> m_handlerList;
    std::mutex m_mutexHandlerList;
    std::shared_ptr<std::thread> m_thread;
    bool running;
    int epollfd;

public:
    void add_client_handler(ClientHandler *handler);
    void init();
    void start();
    void shutdown();
    void thread_func();
};
