#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <stdlib.h>

void my_handler(int nsig) 
{
    (void)nsig;

    int status;
    pid_t pid;

    while (1)
    {
        pid = waitpid(-1, &status, WNOHANG);

        // no more children
        if (pid == 0)
            break;

        // handle error
        if (pid < 0) 
        {
            if (errno == 10) // No child processes
                break;
            printf("Some error on waitpid errno = %d\n", errno);
            break;
        }

        // handle the child
        if ((status & 0xff) == 0) 
        {
            printf("Process %d was exited with status %d\n", pid, status >> 8);
        } 
        else if ((status & 0xff00) == 0) 
        {
            printf("Process %d killed by signal %d %s\n", pid, status &0x7f,
                         (status & 0x80) ? "with core file" : "without core file");
        }
    }

    // reset signal
    (void)signal(SIGCHLD, my_handler);
}

int main(void) 
{
    pid_t pid;

    (void)signal(SIGCHLD, my_handler);

    for (int i = 0; i < 10; i++) 
    {
        if ((pid = fork()) < 0) 
        {
            printf("Can\'t fork child 1\n");
            exit(1);
        } 
        else if (pid == 0) 
        {
            /* Child */
            exit(200 + i);
        }
    }

    while(1);
    return 0;
}
