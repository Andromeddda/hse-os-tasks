#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>

#define VERIFY_CONTRACT(contract, msg) \
if (!(contract)) { \
    printf(msg); \
    exit(-1); \
}

#define LAST_MESSAGE 255 // Message type for termination of program 11-1b.c

struct mymsgbuf
{
    long mtype;
    char mtext[81];
};

int main(void)
{
    int     msqid;
    int     len, maxlen;
    (void)maxlen;

    //
    // Generate key
    //
    key_t key;
    char pathname[] = "src/2a.c";
    VERIFY_CONTRACT((key = ftok(pathname,0)) >= 0, "Can\'t generate key\n");

    //
    // Create queue
    //
    VERIFY_CONTRACT((msqid = msgget(key, 0666 | IPC_CREAT)) >= 0, "Can\'t get msqid\n");

    //
    // Initialize buffer
    //
    struct mymsgbuf mybuf1, mybuf2;

    //
    // send
    //
    mybuf1.mtype = 1;
    strcpy(mybuf1.mtext, "This is text message");
    len = strlen(mybuf1.mtext) + 1;

    if (msgsnd(msqid, (struct msgbuf *) &mybuf1, len, 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    //
    // recieve
    //
    maxlen = 81;
    VERIFY_CONTRACT((len = msgrcv(msqid, (struct msgbuf *)&mybuf2, maxlen, 2, 0)) >= 0, 
        "Can\'t receive message from queue\n");

    if (mybuf2.mtype == LAST_MESSAGE) {
        printf("Recieved 255, removing queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
        exit(0);
    }

    printf("2a: message type = %ld, info = %s\n", mybuf2.mtype, mybuf2.mtext);

    mybuf1.mtype = LAST_MESSAGE;
    len          = 0;

    if (msgsnd(msqid, (struct msgbuf *) &mybuf1, len, 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    return 0;
}
