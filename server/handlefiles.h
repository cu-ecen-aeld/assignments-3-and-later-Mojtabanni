#include <unistd.h>

int open_file(char *pathname);

ssize_t write_file(int pfd, const void *buffer, size_t count);

ssize_t read_file(int pfd, void *buffer, size_t nbytes);

int close_file(int pfd);

int clear_file(char *pathname);

