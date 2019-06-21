#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <vector>
#include <errno.h>

#define INVALID_FD -1

typedef struct sockaddr sa;
typedef struct sockaddr_in sa_in;

int main()
{
    // 创建一个socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        std::cout << "create listen socket error." << std::endl;
        return -1;
    }

    //初始化服务器
    sa_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(5000);
    if (bind(listenfd, (sa *)&bindaddr, sizeof(bindaddr)) == -1)
    {
        std::cout << "bind listen socket error." << std::endl;
        close(listenfd);
        return -1;
    }

    // 启动监听
    if (listen(listenfd, SOMAXCONN) == -1)
    {
        std::cout << "listen error." << std::endl;
        close(listenfd);
        return -1;
    }

    std::vector<int> clientfds;
    int maxfd = listenfd;

    while (true)
    {
        fd_set readset;
        FD_ZERO(&readset);

        FD_SET(listenfd, &readset);
        int clientfdslength = clientfds.size();
        for (int i = 0; i < clientfdslength; i++)
        {
            if (clientfds[i] != INVALID_FD)
            {
                FD_SET(clientfds[i], &readset);
            }
        }

        timeval tm;
        tm.tv_sec = 1;
        tm.tv_usec = 0;

        int ret = select(maxfd + 1, &readset, NULL, NULL, &tm);
        if (ret == -1)
        {
            // 出错，退出
            if (errno != EINTR)
            {
                break;
            }
        }
        else if (ret == 0)
        {
            // 函数超时
            continue;
        }
        else
        {
            if (FD_ISSET(listenfd, &readset))
            {
                sa_in clientaddr;
                socklen_t clientaddrlen = sizeof(clientaddr);

                int clientfd = accept(listenfd, (sa *)&clientaddr, &clientaddrlen);
                if (clientfd == -1)
                {
                    // 接受连接出错，退出
                    break;
                }
                std::cout << "accept a client connection, fd: " << clientfd << std::endl;
                clientfds.push_back(clientfd);

                // 更新最大fd值，作为select函数的第一个参数
                if (clientfd > maxfd)
                {
                    maxfd = clientfd;
                }
            }
            else
            {
                char recvbuf[64];
                int clientfdslength = clientfds.size();
                for (int i = 0; i < clientfdslength; i++)
                {
                    if (clientfds[i] != -1 && FD_ISSET(clientfds[i], &readset))
                    {
                        memset(recvbuf, 0, sizeof(recvbuf));
                        int length = recv(clientfds[i], recvbuf, 64, 0);
                        if (length <= 0 && errno != EINTR)
                        {
                            std::cout << "recv data error, clientfd: " << clientfds[i] << std::endl;
                            close(clientfds[i]);
                            clientfds[i] = INVALID_FD;
                            continue;
                        }
                        std::cout << "clientfd: " << clientfds[i] << ", recv data: " << recvbuf << std::endl;
                    }
                }
            }
        }
    }
    int clientfdslength = clientfds.size();
    for (int i = 0; i < clientfdslength; i++)
    {
        if (clientfds[i] != INVALID_FD)
        {
            close(clientfds[i]);
        }
    }
    close(listenfd);
    return 0;
}