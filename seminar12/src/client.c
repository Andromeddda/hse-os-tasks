#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define VERIFY_CONTRACT(contract, msg) \
if (!(contract)) { \
    printf(msg); \
    exit(-1); \
}

#define LAST_MESSAGE 255 // Message type for termination of program 11-1b.c

struct msgsend_t
{
    long mtype;
    struct acctext_t {
        double num;
        long id;
    } msg;
};

struct msgaccept_t
{
    long mtype;
    struct sndtext_t  {
        double num;
    } msg;
};


int main(void)
{
    int     msqid;             // IPC descriptor for the message queue
    int     len, maxlen;            // Cycle counter and the length of the informative part of the message

    //
    // Generate key
    //
    key_t   key;
    char    pathname[] = "src/server.c";
    VERIFY_CONTRACT((key = ftok(pathname,0)) >= 0, "Can\'t generate key\n");

    //
    // Create queue
    //
    VERIFY_CONTRACT((msqid = msgget(key, 0666)) >= 0, "Can\'t get msqid\n")

    //
    //
    // Initialize buffer
    struct msgaccept_t msgaccept;
    struct msgsend_t msgsend;

    //
    // Read number from console
    //
    printf("Enter floating point number: ");
    int correct = scanf("%lf", &(msgsend.msg.num));
    VERIFY_CONTRACT(correct == 1, "Incorrect input\n");

    long cliend_id = getpid() + 3;

    msgsend.msg.id = cliend_id;
    msgsend.mtype = 1;

    len = sizeof(msgsend.msg);

    if (msgsnd(msqid, (struct msgbuf *) &msgsend, len, 0) < 0) {
        printf("Can\'t send message to queue\n");
        exit(-1);
    }

    maxlen = sizeof(msgaccept.msg);

    if ((len = msgrcv(msqid, (struct msgbuf *)&msgaccept, maxlen, cliend_id, 0)) < 0) {
        printf("Can\'t receive message from server\n");
        exit(-1);
    }

    printf("Square of the number: %lf\n", msgaccept.msg.num);

    return 0;
}
