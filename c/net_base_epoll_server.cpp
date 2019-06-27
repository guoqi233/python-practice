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

#define INVALID_FD -1
typedef struct sockaddr sa;
typedef struct sockaddr_in sa_in;

int main()
{
    // 创建侦听socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        std::cout << "create listen socket error" << std::endl;
        return -1;
    }

    //设置非阻塞
    int oldSocketFlag = fcntl(listenfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(listenfd, F_SETFL, newSocketFlag) == -1)
    {
        close(listenfd);
        std::cout << "set listenfd no block error." << std::endl;
        return -1;
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
        return -1;
    }

    //启动侦听
    if (listen(listenfd, SOMAXCONN) == -1)
    {
        std::cout << "listen error." << std::endl;
        close(listenfd);
        return -1;
    }

    // 创建epollfd
    int epollfd = epoll_create(1);
    if (epollfd == -1)
    {
        std::cout << "create epollfd error." << std::endl;
        close(listenfd);
        return -1;
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
        return -1;
    }

    int n;
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
            if (epoll_events[i].events & EPOLLIN)
            {
                // listenfd
                if (epoll_events[i].data.fd == listenfd)
                {
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
                            epoll_event client_fd_event;
                            client_fd_event.data.fd = clientfd;
                            client_fd_event.events = EPOLLIN;

                            // 使用ET模式
                            // client_fd_event.events = EPOLLET;

                            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &client_fd_event) != -1)
                            {
                                std::cout << "new client accepted, clientfd: " << clientfd << std::endl;
                            }
                            else
                            {
                                std::cout << "add client fd to epollfd error." << std::endl;
                                close(clientfd);
                            }
                        }
                    }
                }
                //普通可读
                else
                {
                    std::cout << "client fd: " << epoll_events[i].data.fd << "recv data." << std::endl;

                    char ch;
                    //每次只收1个字节
                    int m = recv(epoll_events[i].data.fd, &ch, 1, 0);
                    if (m == 0)
                    {
                        // 对方关闭了socket
                        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1)
                        {
                            std::cout << "client disconnected, clientfd: " << epoll_events[i].data.fd << std::endl;
                        }
                        close(epoll_events[i].data.fd);
                    }
                    else if (m < 0)
                    {
                        // 接收出错
                        if (errno != EWOULDBLOCK && errno != EINTR)
                        {
                            if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) == -1)
                            {
                                std::cout << "client disconnected, clientfd: " << epoll_events[i].data.fd << std::endl;
                            }
                            close(epoll_events[i].data.fd);
                        }
                    }
                    else
                    {
                        //正常接收
                        std::cout << "recv from client: " << epoll_events[i].data.fd << " " << ch << std::endl;
                    }
                }
            }
            else if (epoll_events[i].events & POLLERR) // EPOLLERR POLLERR 都是0x008 可以查看宏定义
            {
                // do nothing
            }
        }
    }

    close(listenfd);
    return 0;
}
