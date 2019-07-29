#include "subReactor.h"

void SubReactor::init()
{
    epollfd = epoll_create(1);
    if (epollfd == -1)
    {
        std::cout << "create epoll error." << std::endl;
    }
}

void SubReactor::start()
{
    running = true;
    m_thread.reset(new std::thread(std::bind(&SubReactor::thread_func, this)));
}

void SubReactor::shutdown()
{
    running = false;
    if (m_thread->joinable())
    {
        m_thread->join();
    }
}

void SubReactor::add_client_handler(ClientHandler *handler)
{
    std::shared_ptr<ClientHandler> sp_handler;
    sp_handler.reset(handler);
    {
        std::lock_guard<std::mutex> guard(m_mutexHandlerList);
        m_handlerList.push_back(sp_handler);
    }
    fd_handler_map[handler->client_fd] = sp_handler;
}

void SubReactor::thread_func()
{
    std::shared_ptr<ClientHandler> sp_handler;
    while (running)
    {
        {
            std::unique_lock<std::mutex> guard(m_mutexHandlerList);
            while (!m_handlerList.empty())
            {
                sp_handler = m_handlerList.front();
                m_handlerList.pop_front();
                epoll_event client_fd_event;
                client_fd_event.data.fd = sp_handler->client_fd;
                client_fd_event.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, sp_handler->client_fd, &client_fd_event);
            }
        }
        int n;
        epoll_event epoll_events[1024];
        n = epoll_wait(epollfd, epoll_events, 1024, 1000);
        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
        }
        else if (n == 0)
        {
            continue;
        }
        else
        {
            for (size_t i = 0; i < n; i++)
            {
                if (epoll_events[i].events & EPOLLIN)
                {
                    int fd = epoll_events[i].data.fd;
                    sp_handler = fd_handler_map[fd];
                    int n = sp_handler->do_recv();
                    if (n <= 0)
                    {
                        fd_handler_map.erase(fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                        sp_handler->do_close();
                    }
                }
            }
        }
    }
}