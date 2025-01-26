#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define VERIFY_CONTRACT(contract, msg) \
if (!(contract)) { \
    printf(msg); \
    exit(-1); \
}

int main(void)
{
    // Try to open a file 
    int     fd;
    VERIFY_CONTRACT(
        (fd = open("myfile", O_RDONLY)) >= 0, 
        "Can\'t open file\n");

    ssize_t size;
    char buf;

    // if 0 returned - EOF, exit the loop
    // if < 0 - error occured
    // if > 0 - correct
    while ((size = read(fd, &buf, 1))) 
    {
        VERIFY_CONTRACT(
            size > 0,
            "Error while reading from file\n");

        printf("%c", buf);
    }
    printf("\n");
  
    VERIFY_CONTRACT(
        close(fd) >= 0,
        "Can\'t close file\n");

  return 0;
}
