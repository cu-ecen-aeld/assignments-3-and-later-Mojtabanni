#include "aesdsocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>

struct addrinfo* addr_info = NULL;
#define LISTEN_BACKLOG 50

int create_socket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int bind_addr(int sockfd, char* port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    // hints.ai_family = AF_UNSPEC;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rc_getaddrinfo = getaddrinfo(NULL, port, &hints, &addr_info); 
    if(rc_getaddrinfo != 0)
        return rc_getaddrinfo;

    return bind(sockfd, addr_info->ai_addr, sizeof(struct addrinfo));
}

int listen_conn(int sockfd)
{
    return listen(sockfd, LISTEN_BACKLOG);
}

int accept_conn(int sockfd, struct sockaddr *addr_cli)
{
    int addrlen = sizeof(*addr_cli);
    return accept(sockfd, addr_cli, (socklen_t *)(&addrlen));
}

void get_ipcli(const struct sockaddr *addr_cli, char *s_ipcli)
{
    struct sockaddr_in *pV4Addr = (struct sockaddr_in *)addr_cli;
    struct in_addr ipcli = pV4Addr->sin_addr;
    char str_ipcli[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipcli, str_ipcli, INET_ADDRSTRLEN);
    strcpy(s_ipcli, str_ipcli);
}

int recv_data(int sockfd, void *buff, int buff_size)
{
    return recv(sockfd, buff, buff_size, 0);
}

int send_data(int sockfd, void *buff, int buff_size)
{
    return send(sockfd, buff, buff_size, 0);
}

void close_socket(int sock)
{
    free(addr_info);
    close(sock);
}
