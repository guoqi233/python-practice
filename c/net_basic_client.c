#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct sockaddr sa;
typedef struct sockaddr_in sa_in;

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 5000
#define SEND_DATA "hello world"

int main(int agrc, char *argv[])
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        printf("create listen socket error.\n");
        return -1;
    }

    sa_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    if (connect(client_fd, (sa *)&server_addr, sizeof(server_addr)) == -1)
    {
        printf("connect server error.\n");
        return -1;
    }

    int ret = send(client_fd, SEND_DATA, strlen(SEND_DATA), 0);
    if (ret != strlen(SEND_DATA))
    {
        printf("send data error.\n");
        return -1;
    }
    printf("send data successfully.\n");
    char recv_buf[32] = {0};
    ret = recv(client_fd, recv_buf, 32, 0);
    if (ret > 0)
    {
        printf("recv echo: %s\n", recv_buf);
    }
    else
    {
        printf("recv data error, data: %s\n", recv_buf);
    }
    close(client_fd);

    return 0;
}