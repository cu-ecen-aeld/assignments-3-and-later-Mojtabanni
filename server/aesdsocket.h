#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

int create_socket();

int bind_addr(int sockfd, char* port);

int listen_conn(int sockfd);

int accept_conn(int sockfd, struct sockaddr *addr_cli);

void get_ipcli(const struct sockaddr *addr_cli, char *s_ipcli);

int recv_data(int sockfd, void *buff, int buff_size);

int send_data(int sockfd, void *buff, int buff_size);

void close_socket(int sock);