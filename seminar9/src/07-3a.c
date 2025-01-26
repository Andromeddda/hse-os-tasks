#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <sys/sem.h>

#define VERIFY_CONTRACT(contract, msg) \
if (!(contract)) { \
    printf(msg); \
    exit(-1); \
}

#define VERIFY_CONTRACT_F(contract, format, ...) \
if (!(contract)) { \
    printf(format, __VA_ARGS__); \
    exit(-1); \
}

int main(void)
{
    int         *array;
    int         shmid, semid;
    int         new = 1, sem_new = 1;
    char        pathname[] = "src/07-3a.c";
    // char        spathname[] = "src/07-3b.c";
    key_t       key;

    //
    // Create shared memory
    //
    VERIFY_CONTRACT((key = ftok(pathname,0)) >= 0, "Can\'t generate key\n");

    if ((shmid = shmget(key, 3*sizeof(int), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
        VERIFY_CONTRACT(errno == EEXIST, "Can\'t create shared memory\n");
        VERIFY_CONTRACT(
            (shmid = shmget(key, 3*sizeof(int), 0)) >= 0, 
            "Can\'t find shared memory\n");
        new = 0;
    }

    VERIFY_CONTRACT((array = (int *)shmat(shmid, NULL, 0)) != (int *)(-1), "Can't attach shared memory\n");

    
    //
    // Create semaphore
    //

    // VERIFY_CONTRACT((key = ftok(spathname,0)) >= 0, "Can\'t generate key\n");

    if ((semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
        VERIFY_CONTRACT(errno == EEXIST, "Can\'t create semaphore\n");
        VERIFY_CONTRACT(
            (semid = semget(key, 1, 0)) >= 0, 
            "Can\'t find semaphore\n");
        sem_new = 0;
    }

    //
    // Perform operations
    //

    struct sembuf sem_unlock;
    struct sembuf sem_lock;

    sem_unlock.sem_num = sem_lock.sem_num = 0;
    sem_unlock.sem_op = 1;
    sem_lock.sem_op = -1;
    sem_unlock.sem_flg = sem_lock.sem_flg = 0;


    if (sem_new)
        VERIFY_CONTRACT(semop(semid, &sem_unlock, 1) != -1, "Can\'t perform semop (1)\n");

    if (new) {
        array[0] =    1;
        array[1] =    0;
        array[2] =    1;

        printf("Program 1 was spawn %d times, program 2 - %d times, total - %d times\n",
        array[0], array[1], array[2]);

    } else {
        /*lock*/ VERIFY_CONTRACT(semop(semid, &sem_lock, 1) != -1, "Can\'t perform semop (2)\n");

        array[0] += 1;
        for(long i = 0; i < 2000000000L; i++);
        array[2] += 1;

        printf("Program 1 was spawn %d times, program 2 - %d times, total - %d times\n",
        array[0], array[1], array[2]);

        /*unlock*/ VERIFY_CONTRACT(semop(semid, &sem_unlock, 1) != -1, "Can\'t perform semop (3)\n");
    }


    // Detach shared memory
    VERIFY_CONTRACT(shmdt(array) >= 0, "Can't detach shared memory\n");

    return 0;
}
