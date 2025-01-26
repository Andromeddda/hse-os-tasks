#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define VERIFY_CONTRACT(contract, msg) \
if (!(contract)) { \
    printf(msg); \
    exit(-1); \
}

int main(void) {
    (void)umask(0);

    // Create fifo
    char      name[] = "aaa.fifo";
    VERIFY_CONTRACT((mkfifo(name, 0666) >= 0) || (errno == EEXIST), "Can\'t create FIFO\n");

    // Open file on write
    int fd;
    VERIFY_CONTRACT((fd = open(name, O_WRONLY)) >= 0, "Can\'t open FIFO for writting\n");

    // Write
    ssize_t size = write(fd, "Hello, world!", 14);
    VERIFY_CONTRACT(size == 14, "Can\'t write all string to FIFO\n");
    VERIFY_CONTRACT(close(fd) >= 0, "Can\'t close FIFO\n");

    printf("Child1 exit\n");
    return 0;
}
