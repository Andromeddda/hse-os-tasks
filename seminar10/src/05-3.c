// standart library
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

// sys
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <fcntl.h>

// ===================== //
// ===== ALGORITHM ===== //
// ===================== //

// Initial value of the semaphore
// S = 1

// FIRST PROCESS
// loop:
//      write
//      D(S, 1)
//      D(S, 1)
//      read

// SECOND PROCESS
// loop:
//      Z(S)
//      read
//      write
//      A(S, 2)

// Explanation:
//  1. S = 1
//  2. Both processes enter their loops
//  3. Process 2 blocks on Z(S)
//  4. Process 1 writes to pipe, performs first D(S, 1) and blocks on second D(S, 1)
//  5. S = 0
//  6. Process 2 unblocks after first D(S, 1), reads and writes to pipe, then A(S, 2)
//  7. S = 2
//  8. Process 1 unblocks, performs D(S, 1) and reads pipe
//  9. S = 1 ---> go to step 1. 

#define VERIFY_CONTRACT(contract, msg) \
if (!(contract)) { \
    printf(msg); \
    exit(-1); \
}

int main(void)
{
    ssize_t size;

    //
    //  Read the number of communication cycles from the console
    //
    printf("How many loops?\t");
    int    N;
    VERIFY_CONTRACT(scanf("%u", &N) == 1, "Incorrect input, expected positive integer\n");

    //
    // Open pipe
    //
    int     fd[2];
    VERIFY_CONTRACT(pipe(fd) >= 0, "Can\'t open pipe\n");

    //
    // Get rid of blocking
    //
    int flags;

    flags = fcntl(fd[0], F_GETFL);
    fcntl(fd[0], F_SETFL, flags | O_NONBLOCK);

    flags = fcntl(fd[1], F_GETFL);
    fcntl(fd[1], F_SETFL, flags | O_NONBLOCK);

    //
    // Create a semaphore
    //
    int key;
    char pathname[] = "src/05-3.c";
    VERIFY_CONTRACT((key = ftok(pathname,0)) >= 0, "Can\'t generate key\n");

    int semid;
    VERIFY_CONTRACT((semid = semget(key, 1, 0666 | IPC_CREAT)) >= 0,  "Can\'t create semaphore\n");

    //
    // Initialize semaphore operations
    //
    struct sembuf sem_init, sem_Z, sem_A2, sem_D1;

    sem_init.sem_num = sem_Z.sem_num = sem_A2.sem_num = sem_D1.sem_num = 0;
    sem_init.sem_flg = sem_Z.sem_flg = sem_A2.sem_flg = sem_D1.sem_flg = 0;

    sem_D1.sem_op  = -1;
    sem_Z.sem_op   = 0;
    sem_init.sem_op = 1;
    sem_A2.sem_op   = 2;

    VERIFY_CONTRACT(semop(semid, &sem_init, 1) != -1, "Can\'t perform semop (0)\n");

    char message[27];

    //
    // Fork a child
    //
    pid_t pid = fork();
    if (pid  < 0) {
        printf("Can\'t fork child\n");
        exit(-1);
    } else if (pid > 0) {
    /* Parent process */
        for (int i = 0; i < N; i++) {
            /* write */
            strcpy(message, "My favorite letter is \'A\'\n");
            message[23] = (char)((2*i % 26) + 'A');

            printf("PARENT SAYS: %s", message);

            size = write(fd[1], message, 27);
            VERIFY_CONTRACT(size == 27, "Can\'t write all string to pipe\n");

            /* D(S,1) */ VERIFY_CONTRACT(semop(semid, &sem_D1, 1) != -1, "Can\'t perform semop (1)\n");
            /* D(S,1) */ VERIFY_CONTRACT(semop(semid, &sem_D1, 1) != -1, "Can\'t perform semop (2)\n");

            /* read */
            size = read(fd[0], message, 27);
            VERIFY_CONTRACT(size >= 0, "Can\'t read string from pipe\n");

            printf("\nPARENT HEARS: %s", message);
        }


        printf("PARENT SAYS GOODBYE\n");
    } else {
    /* Child process */

        for (int i = 0; i < N; i++) {
            /* Z(S) */ VERIFY_CONTRACT(semop(semid, &sem_Z, 1) != -1, "Can\'t perform semop (3)\n");

            /* read */
            size = read(fd[0], message, 27);
            VERIFY_CONTRACT(size >= 0, "Can\'t read string from pipe\n");

            printf("\nCHILD HEARS: %s", message);

            /* write */
            strcpy(message, "My favorite letter is \'A\'\n");
            message[23] = (char)(((2*i + 1) % 26) + 'A');

            printf("CHILD SAYS: %s", message);

            size = write(fd[1], message, 27);
            VERIFY_CONTRACT(size == 27, "Can\'t write all string to pipe\n");

            /* A(S,2) */ VERIFY_CONTRACT(semop(semid, &sem_A2, 1) != -1, "Can\'t perform semop (4)\n");
        }

        printf("CHILD SAYS GOODBYE\n");
    }

    //
    // Delete the semaphore
    //
    semctl(semid, IPC_RMID, 0);

    return EXIT_SUCCESS;
}
