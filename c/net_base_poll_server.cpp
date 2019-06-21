#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <string.h>
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

    std::vector<pollfd> fds;
    pollfd listen_fd_info;
    listen_fd_info.fd = listenfd;
    listen_fd_info.events = POLLIN;
    listen_fd_info.revents = 0;
    fds.push_back(listen_fd_info);

    bool exist_invalid_fd;
    int n;
    while (true)
    {
        exist_invalid_fd = false;
        n = poll(&fds[0], fds.size(), 1000);
        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            break;
        }
        else if (n == 0)
        {
            continue;
        }
        for (int i = 0; i < fds.size(); i++)
        {
            // 可读事件
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == listenfd)
                {
                    // 侦听socket，接受新的连接
                    sa_in clientaddr;
                    socklen_t clientaddr_len = sizeof(clientaddr);
                    int clientfd = accept(listenfd, (sa *)&clientaddr, &clientaddr_len);
                    if (clientfd != -1)
                    {
                        int oldClientFlag = fcntl(clientfd, F_GETFL, 0);
                        int newClientFlag = oldClientFlag | O_NONBLOCK;
                        if (fcntl(clientfd, F_SETFL, newClientFlag) == -1)
                        {
                            close(clientfd);
                            std::cout << "set clientfd noblock error." << std::endl;
                        }
                        else
                        {
                            struct pollfd client_fd_info;
                            client_fd_info.fd = clientfd;
                            client_fd_info.events = POLLIN;
                            client_fd_info.revents = 0;
                            fds.push_back(client_fd_info);
                            std::cout << "new client accepted, clientfd: " << clientfd << std::endl;
                        }
                    }
                }
                else
                {
                    //普通socket 接收数据
                    /*
                    char recv_buf[64];
                    memset(recv_buf, 0, sizeof(recv_buf));
                     */
                    char recv_buf[64] = {0};
                    int length = recv(fds[i].fd, recv_buf, sizeof(recv_buf), 0);
                    if (length <= 0)
                    {
                        if (errno != EINTR && errno != EWOULDBLOCK)
                        {
                            for (std::vector<pollfd>::iterator iter = fds.begin(); iter != fds.end(); iter++)
                            {
                                if (iter->fd == fds[i].fd)
                                {
                                    std::cout << "client disconnected, clientfd: " << fds[i].fd << std::endl;
                                    close(fds[i].fd);
                                    iter->fd = INVALID_FD;
                                    exist_invalid_fd = true;
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        std::cout << "recv from client: " << recv_buf << ", clientfd: " << fds[i].fd << std::endl;
                    }
                }
            }
            else if (fds[i].revents & POLLERR)
            {
                //do nothing
            }
        }
        if (exist_invalid_fd)
        {
            for (std::vector<pollfd>::iterator iter = fds.begin(); iter != fds.end();)
            {
                if (iter->fd == INVALID_FD)
                {
                    iter = fds.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }
    }
    //关闭所有socket
    for (std::vector<pollfd>::iterator iter = fds.begin; iter != fds.end(); ++iter)
    {
        close(iter->fd);
    }
    return 0;
}