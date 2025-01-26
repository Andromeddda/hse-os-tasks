#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define VERIFY_CONTRACT(contract, msg) \
if (!(contract)) { \
    printf(msg); \
    exit(-1); \
}

#define MSG1_SIZE 28
#define MSG2_SIZE 38

int main(void)
{   
    // fd1 - from parent to child
    // fd2 - from child to parent
    int fd1[2], fd2[2];

    VERIFY_CONTRACT(
        (pipe(fd1) >= 0) && (pipe(fd2) >= 0),
        "Can\'t open pipe\n");

    ssize_t wsize, rsize;
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("Can\'t fork child\n");
        exit(-1);
    } 
    else if (pid > 0) 
    {
    /* Parent process */
    /* read from fd2, write to fd1*/

        // Close unnecessary pipe ends
        VERIFY_CONTRACT(close(fd1[0]) >= 0, "parent: can\'t close reading side of pipe 1\n");
        VERIFY_CONTRACT(close(fd2[1]) >= 0, "parent: can\'t close writing side of pipe 2\n");

        // write to fd1
        wsize = write(fd1[1], "Is there anybody out there?", MSG1_SIZE);
        VERIFY_CONTRACT(wsize == MSG1_SIZE, "parent: can\'t write all string to pipe\n");

        // read from fd2
        char    buf[MSG2_SIZE];
        rsize = read(fd2[0], buf, MSG2_SIZE);
        VERIFY_CONTRACT(rsize >= 0, "parent: can't read string from pipe\n");

        // Close remaining pipe ends
        VERIFY_CONTRACT(close(fd1[1]) >= 0, "parent: can\'t close writing side of pipe 1\n");
        VERIFY_CONTRACT(close(fd2[0]) >= 0, "parent: can\'t close reading side of pipe 2\n");

        printf("Parent exit, accepted message: %s\n", buf);
    } 
    else 
    {
    /* Child process */
    /* read from fd1, write to fd2*/

        // Close unnecessary pipe ends
        VERIFY_CONTRACT(close(fd1[1]) >= 0, "child: can\'t close writing side of pipe 1\n");
        VERIFY_CONTRACT(close(fd2[0]) >= 0, "child: can\'t close reading side of pipe 2\n");

        // write to fd2
        wsize = write(fd2[1], "Does anybody here remember Vera Lynn?", MSG2_SIZE);
        VERIFY_CONTRACT(wsize == MSG2_SIZE, "child: can\'t write all string to pipe\n");

        // read from fd1
        char    buf[MSG1_SIZE];
        rsize = read(fd1[0], buf, MSG1_SIZE);
        VERIFY_CONTRACT(rsize >= 0, "child: can't read string from pipe\n");

        // Close remaining pipe ends
        VERIFY_CONTRACT(close(fd1[0]) >= 0, "child: Can\'t close reading side of pipe 1\n");
        VERIFY_CONTRACT(close(fd2[1]) >= 0, "child: Can\'t close writing side of pipe 2\n");
        
        printf("Child exit, accepted message: %s\n", buf);
    }

    return 0;
}
