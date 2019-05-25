#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>

#define LISTEN_PORT 9999
#define ISspace(x) isspace((int)(x))
#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"

typedef struct sockaddr sa;
typedef struct sockaddr_in sa_in;


int get_line(int sock, char* buf, int size);
void serve_file(int client, const char *filename);
void unimplemented(int client);
void not_found(int client);
void headers(int client, const char *filename);
void cat(int client, FILE *resource);


void headers(int client, const char *filename)
{
    char buf[1024];
    (void)filename;

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

void cat(int client, FILE *resource)
{
  char buf[1024];

  fgets(buf, sizeof(buf), resource);
  while (!feof(resource))
  {
    send(client, buf, strlen(buf), 0);
    fgets(buf, sizeof(buf), resource);
  }
}

void errExit(const char *s)
{
    perror(s);
    exit(1);
}
void info(const char *msg)
{
    printf("%s\n", msg);
}

int startup(int *port)
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
    {
        errExit("listen error");
    }
    info("create socket...");
    sa_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = (htons(*port));

    if (bind(listen_fd, (sa *)&bindaddr, sizeof(bindaddr)) == -1)
    {
        errExit("error bind");
    }
    info("bind...");
    if (listen(listen_fd, SOMAXCONN) == -1)
    {
        errExit("error listen");
    }
    info("listen...");
    return listen_fd;
}

void* thread_fun(void *vargp)
{
    printf("tid is: %ld\n", pthread_self());
    return NULL;
}

void server_file(int client_fd, const char *filename)
{
    FILE *fp = NULL;
    int numchars = 1;
    char buf[1024] = {0};

    buf[0] = 'A';
    buf[1] = '\0';
    while ((numchars > 0) && strcmp("\n", buf))
    {
        numchars = get_line(client_fd, buf, sizeof(buf));
    }
    fp = fopen(filename, O_RDONLY);
    if (fp == NULL)
    {
        not_found(client_fd);
    }
    else
    {
        headers(client_fd, filename);
        cat(client_fd, fp);
    }
    fclose(fp);
}

void unimplemented(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</TITLE></HEAD>\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
}

void not_found(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "your request because the resource specified\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "is unavailable or nonexistent.\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
}

void* accept_request(void* arg)
{   
    pthread_detach(pthread_self());
    int client_fd = *(int *)arg;
    char buf[1024] = {0};
    int numchars;
    char methods[255] = {0};
    char url[1024] = {0};
    char path[1024] = {0};
    size_t i, j = 0;
    struct stat st;
    int cgi = 0;
    char* query_string = NULL;
    numchars = get_line(client_fd, buf, sizeof(buf));
    i = 0;
    while(!ISspace(buf[j]) && (i < sizeof(methods) -1))
    {
        /*
        获取http请求方法，从开始到第一个空格为止
        */
        methods[i] = buf[j];
        i++;
        j++;
    }
    methods[i] = '\0';
    if(strcasecmp(methods, "GET") && strcasecmp(methods, "POST"))
    {
        unimplemented(client_fd);
        return;
    }

    if(strcasecmp(methods, "POST") == 0)
    {
        cgi = 1;
    }

    while (ISspacce(buf[j]) && (j < sizeof(buf)))
    {
        j++;
    }

    i = 0;
    while (!ISspace(buf[j]) && (i < sizeof(url) -1))
    {
        /*
        匹配路由
        */
        url[i] = buf[j];
        i++;
        j++;
    }
    url[i] = '\0';

    if(strcasecmp(methods, "GET") == 0)
    {
        query_string = url;
        while((*query_string != '?') && (*query_string != '\0'))
        {
            query_string++;
        }
        if(*query_string == '?')
        {
            cgi = 1;
            *query_string = '\0';
            query_string++;
        }
    }
    sprintf(path, "htocs%s", url);
    if(path[strlen(path)] == '/')
    {
        strcat(path, "index.html");
    }
    if(stat(path, &st) == -1)
    {
        while ((numchars > 0) && strcmp("\n", buf))
        {
            numchars = get_line(client_fd, buf, sizeof(buf));
        }
        not_found(client_fd);
    }
    else
    {
        if((st.st_mode & __S_IFMT) == __S_IFDIR)
        {
            strcat(path, "/index.html");
        }
        if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
        {
            cgi = 1;
        }
        if(!cgi)
        {
            serve_file(client_fd, path);
        }
        else
        {
            
        }
        
    }
    close(client_fd);
    free(arg);
    printf("over\n");
}

int get_line(int sock, char* buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    while ((i < size -1) && (c != '\n'))
    {
        n =  recv(sock, &c, 1, 0);
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                if((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buf[i] = '\0';
    return i;
}


int main(int argc, char *argv[])
{
    printf("pid is: %d\n", getpid());
    int port = LISTEN_PORT;
    int server_fd = startup(&port);
    pthread_t tid;
    while (1)
    {
        sa_in clientaddr;
        socklen_t client_addr_len = sizeof(clientaddr);
        int* client_fd;
        client_fd = (int*) malloc(sizeof(int));
        *client_fd = accept(server_fd, (sa *)&clientaddr, &client_addr_len);
        if (*client_fd != -1)
        {
            if(pthread_create(&tid, NULL, accept_request, client_fd) != 0)
            {
                perror("thread create error");
            }
        }
        else
        {
            printf("accept error\ncontinue...\n");
        }
    }
    if (close(server_fd) == -1)
    {
        errExit("close error");
    }

    return 0;
}