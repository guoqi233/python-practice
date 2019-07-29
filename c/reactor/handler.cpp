#include "handler.h"

void ClientHandler::init(int fd)
{
    client_fd = fd;
    std::cout <<"create fd handler "<< client_fd << std::endl;
}

int ClientHandler::do_recv()
{
    int n = recv(client_fd, &buffer, 1024, 0);
    if (n > 0)
        recvd_len += n;
    buffer[n] = '\0';
    std::cout << buffer;
    return n;
}

void ClientHandler::do_close()
{
    close(client_fd);
    std::cout <<"close fd "<< client_fd << std::endl;
}

ClientHandler::~ClientHandler()
{
    std::cout <<"free fd handler "<< client_fd << std::endl;
}
