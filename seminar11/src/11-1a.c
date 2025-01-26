#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define VERIFY_CONTRACT(contract, msg) \
if (!(contract)) { \
    printf(msg); \
    exit(-1); \
}

#define LAST_MESSAGE 255 // Message type for termination of program 11-1b.c

int main(void)
{
    int     msqid;            // IPC descriptor for the message queue
    int     i,len;            // Cycle counter and the length of the informative part of the message

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


    /* Send information */
    for (i = 1; i <= 5; i++) {
        //
        // Fill in the structure for the message and
        // determine the length of the informative part.
        //
        mybuf.mtype = 1;
        strcpy(mybuf.mtext.str, "This is text message");
        mybuf.mtext.a = 1234;
        len = sizeof(mybuf.mtext);
        //
        // Send the message. If there is an error,
        // report it and delete the message queue from the system.
        //

        if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0) {
            printf("Can\'t send message to queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }
    }

    /* Send the last message */

    mybuf.mtype = LAST_MESSAGE;
    len                 = 0;

    if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    return 0;
}
