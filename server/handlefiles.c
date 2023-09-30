#include "handlefiles.h"
#include <fcntl.h>
#include <sys/stat.h>

int open_file(char *pathaname)
{
    return open(pathaname, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
}

ssize_t write_file(int pfd, const void *buffer, size_t count)
{
    return write(pfd, buffer, count);
}

ssize_t read_file(int pfd, void *buffer, size_t nbytes)
{
    return read(pfd, buffer, nbytes);
}

int close_file(int pfd)
{
    return close(pfd);
}

int clear_file(char *pathname)
{
    int pfd = open(pathname, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (pfd > -1)
    {
        close(pfd);
    }
    return pfd;
}