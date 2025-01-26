#include <signal.h>
#include <stdio.h>

void my_handler(int nsig) 
{
    printf("<Recieved signal %d>\tDo you think this is a hit?\n", nsig);

    // behaviour is set to default now
    // reset it

    (void)signal(SIGINT, my_handler);
    (void)signal(SIGQUIT, my_handler);
}

int main(void) {
    //
    // Set the process response to the SIGINT and SIGQUIT signal
    //

    (void)signal(SIGINT, my_handler);
    (void)signal(SIGQUIT, my_handler);

    //
    // From this point, the process will print SIGINT and SIGQUIT message.
    //
    while(1);
    return 0;
}
