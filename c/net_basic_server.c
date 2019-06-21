#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct sockaddr sa;
typedef struct sockaddr_in sa_in;

int main(int agrc, char *argv[])
{
    // 创建 socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
    {
        printf("create listen socket error.\n");
        return -1;
    }

    // 初始化服务器地址
    sa_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(5000);
    if (bind(listen_fd, (sa *)&bindaddr, sizeof(bindaddr)) == -1)
    {
        printf("bind listen sock error.\n");
        return -1;
    }

    // 监听
    if (listen(listen_fd, SOMAXCONN) == -1)
    {
        printf("listen error.\n");
        return -1;
    }

    while (1)
    {
        sa_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        // 接受客户端连接
        int client_fd = accept(listen_fd, (sa *)&client_addr, &client_addr_len);
        if (client_fd != -1)
        {
            char recv_buf[32] = {0};
            int ret = recv(client_fd, recv_buf, 32, 0);
            if (ret > 0)
            {
                printf("recv data from client: %s\n", recv_buf);
                ret = send(client_fd, recv_buf, strlen(recv_buf), 0);
                if (ret != strlen(recv_buf))
                {
                    printf("send data error.\n");
                }
            }
            else
            {
                printf("recv data error.");
            }
            close(client_fd);
        }
    }
    close(listen_fd);
    return 0;
}