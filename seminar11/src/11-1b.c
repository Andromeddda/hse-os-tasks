#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LAST_MESSAGE 255

#define VERIFY_CONTRACT(contract, msg) \
if (!(contract)) { \
    printf(msg); \
    exit(-1); \
}

int main(void)
{
    int     msqid;
    int     len, maxlen;

    struct mymsgbuf // Custom structure for the message
    {
        long mtype;
        struct inner {
            char str[81];
            int a;
        } mtext;
    } mybuf;

    //
    // Generate key
    //
    key_t   key;
    char    pathname[] = "src/11-1a.c";
    VERIFY_CONTRACT((key = ftok(pathname,0)) >= 0, "Can\'t generate key\n");

    //
    // Create queue
    //
    VERIFY_CONTRACT((msqid = msgget(key, 0666 | IPC_CREAT)) >= 0, "Can\'t get msqid\n");

    while (1) {
        //
        // In an infinite loop, accept messages of any type in FIFO order
        // with a maximum length of 81 characters
        // until a message of type LAST_MESSAGE is received.
        //
        maxlen = sizeof(mybuf.mtext);

        if ((len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, 0, 0)) < 0) {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }
        //
        // If the received message is of type LAST_MESSAGE,
        // then terminate and remove the message queue from the system.
        // Otherwise, print the text of the received message.
        //
        if (mybuf.mtype == LAST_MESSAGE) {
            msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            exit(0);
        }

        printf("message type = %ld, str = %s, a = %d\n", mybuf.mtype, mybuf.mtext.str, mybuf.mtext.a);
    }

    return 0;
}
