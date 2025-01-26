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

struct msgaccept_t
{
    long mtype;
    struct acctext_t {
        double num;
        long id;
    } msg;
};

struct msgsend_t
{
    long mtype;
    struct sndtext_t {
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
    VERIFY_CONTRACT((msqid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL)) >= 0, "Can\'t get msqid\n");

    struct msgaccept_t msgaccept;
    struct msgsend_t msgsend;


    while (1) {
        maxlen = sizeof(msgaccept.msg);

        if ((len = msgrcv(msqid, (struct msgbuf *) &msgaccept, maxlen, 1, 0)) < 0) {
            printf("Can\'t receive message from client\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }

        if (msgaccept.mtype == LAST_MESSAGE) {
            msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
            exit(0);
        }

        double cliend_num = msgaccept.msg.num;
        long cliend_id = msgaccept.msg.id;

        printf("recieved %lf\n from client %ld\n", cliend_num, cliend_id);

        msgsend.mtype = cliend_id;
        msgsend.msg.num = cliend_num * cliend_num;

        len = sizeof(msgsend.msg);

        if (msgsnd(msqid, (struct msgbuf *) &msgsend, len, 0) < 0) {
            printf("Can\'t send message to queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }
    }

    return 0;
}
