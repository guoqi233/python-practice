#include "reactor.h"

void Reactor::init()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        std::cout << "create listen socket error" << std::endl;
    }

    //设置非阻塞
    int oldSocketFlag = fcntl(listenfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(listenfd, F_SETFL, newSocketFlag) == -1)
    {
        close(listenfd);
        std::cout << "set listenfd no block error." << std::endl;
    }

    //设置端口地址复用
    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

    //初始化服务器地址
    sa_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(8000);
    if (bind(listenfd, (sa *)&bindaddr, sizeof(bindaddr)) == -1)
    {
        std::cout << "bind listen sockt error." << std::endl;
        close(listenfd);
    }

    //启动侦听
    if (listen(listenfd, SOMAXCONN) == -1)
    {
        std::cout << "listen error." << std::endl;
        close(listenfd);
    }

    // 创建epollfd
    epollfd = epoll_create(1);
    if (epollfd == -1)
    {
        std::cout << "create epollfd error." << std::endl;
        close(listenfd);
    }

    epoll_event listen_fd_event;
    listen_fd_event.data.fd = listenfd;
    listen_fd_event.events = EPOLLIN;

    //使用ET模式
    // listen_fd_event.events = EPOLLET

    // 将监听socket绑定到epollfd上
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &listen_fd_event) == -1)
    {
        std::cout << "epoll_ctl error" << std::endl;
        close(listenfd);
    }
    subreactor = new SubReactor();
    subreactor->init();
    subreactor->start();
}

void Reactor::start()
{
    int n;
    std::cout << "listening..." << std::endl;
    while (true)
    {
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
        for (size_t i = 0; i < n; i++)
        {
            //事件可读
            if ((epoll_events[i].events & EPOLLIN) && epoll_events[i].data.fd == listenfd)
            {
                // listenfd
                sa_in clientaddr;
                socklen_t clientaddrlen = sizeof(clientaddr);
                int clientfd = accept(listenfd, (sa *)&clientaddr, &clientaddrlen);
                if (clientfd != -1)
                {
                    int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
                    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
                    if (fcntl(clientfd, F_SETFL, newSocketFlag) == -1)
                    {
                        close(clientfd);
                        std::cout << "set clientfd no block error." << std::endl;
                    }
                    else
                    {
                        ClientHandler *handler;
                        handler = new ClientHandler();
                        handler->init(clientfd);
                        subreactor->add_client_handler(handler);
                    }
                }
            }
        }
    }
}