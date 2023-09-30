#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define PORT 9000
#define BUFF_SIZE 1024
#define PCKT_SIZE 20 * 1024
#define FILE_PATH "/var/tmp/aesdsocketdata"

bool accept_conn_loop = true;

#define CHECK_EXIT_CONDITION(rc, func_name) \
    do                                      \
    {                                       \
        if ((rc) == -1)                     \
        {                                   \
            exit(EXIT_FAILURE);             \
        }                                   \
    } while (0)

static void signal_handler(int signal_number)
{
    if ((signal_number == SIGINT) || (signal_number == SIGTERM))
    {
        syslog(LOG_INFO, "Caught signal, exiting");
        accept_conn_loop = false;
    }
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

void _daemon()
{
    // PID: Process ID
    // SID: Session ID
    pid_t pid, sid;
    pid = fork(); // Fork off the parent process
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }
    // Create a SID for child
    sid = setsid();
    if (sid < 0)
    {
        // FAIL
        exit(EXIT_FAILURE);
    }
    if ((chdir("/")) < 0)
    {
        // FAIL
        exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(int argc, char **argv)
{
    /// create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    CHECK_EXIT_CONDITION(sockfd, "create_socket");

    char port[5];
    memset(port, 0, sizeof port);
    sprintf(port, "%d", PORT);

    struct addrinfo* addr_info = NULL;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rc_bind = getaddrinfo(NULL, port, &hints, &addr_info); 
    if(rc_bind == 0)
        rc_bind = bind(sockfd, addr_info->ai_addr, sizeof(struct addrinfo));

    CHECK_EXIT_CONDITION(rc_bind, "bind_addr");

    openlog("server_log", LOG_PID, LOG_USER);

    struct sigaction new_action;
    memset((void *)&new_action, 0, sizeof(struct sigaction));
    new_action.sa_handler = signal_handler;
    if ((sigaction(SIGTERM, &new_action, NULL) != 0) || (sigaction(SIGINT, &new_action, NULL) != 0))
    {
        return 0;
    }

    if (argc == 2 && strcmp(argv[1], "-d") == 0)
    {
        _daemon();
    }

    int rc_listen = listen(sockfd, 50);
    CHECK_EXIT_CONDITION(rc_listen, "listen_conn");

    int pfd = open(FILE_PATH, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    CHECK_EXIT_CONDITION(pfd, "open_file");

    while (accept_conn_loop)
    {
        struct sockaddr addr_cli;
        int connfd = accept_conn(sockfd, &addr_cli);
        if (connfd == -1)
        {
            shutdown(sockfd, SHUT_RDWR);
            continue;
        }

        char str_ipcli[BUFF_SIZE];
        get_ipcli(&addr_cli, str_ipcli);
        syslog(LOG_INFO, "Accepted connection from %s", str_ipcli);

        while (true)
        {
            /// receive data - write to file
            char recv_buff[BUFF_SIZE + 1];
            memset((void *)recv_buff, 0, BUFF_SIZE + 1);
            int rc_recvdata = recv(connfd, recv_buff, BUFF_SIZE, 0);
            CHECK_EXIT_CONDITION(rc_recvdata, "recv_data");

            int rc_writefile = write(pfd, (const void *)recv_buff, rc_recvdata);
            CHECK_EXIT_CONDITION(rc_writefile, "write_file");

            char *pch = strstr(recv_buff, "\n");
            if (pch != NULL)
                break;
        }

        int data_size = lseek(pfd, 0L, SEEK_CUR);
        char send_buff[BUFF_SIZE];
        lseek(pfd, 0L, SEEK_SET);
        do
        {
            int rc_readfile = read(pfd, send_buff, BUFF_SIZE);
            CHECK_EXIT_CONDITION(rc_readfile, "read_file");
            int rc_senddata = send(connfd, send_buff, rc_readfile, 0);
            CHECK_EXIT_CONDITION(rc_senddata, "send_data");
            data_size -= rc_readfile;
            memset(send_buff, 0, BUFF_SIZE);
        } while (data_size > 0);

        syslog(LOG_INFO, "Closed connection from %s", str_ipcli);
    }

    /// shutdown
    close(pfd);
    closelog();
    remove(FILE_PATH);

    return 0;
}