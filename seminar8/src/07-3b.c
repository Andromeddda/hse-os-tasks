#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>

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

struct peterson_mutex {
    bool     want_[2];
    int      victim_;
};

typedef struct peterson_mutex Mutex;

void    mutex_init(Mutex *mtx);
void    mutex_lock(Mutex *mtx, int thrd);
void    mutex_unlock(Mutex *mtx, int thrd);

int main(void)
{
    int         *array;
    int         shmid;
    int         new = 1;
    char        pathname[] = "src/07-3b.c";
    key_t       key;
    long        i;


    VERIFY_CONTRACT((key = ftok(pathname,0)) >= 0, "Can\'t generate key\n");

    if ((shmid = shmget(key, 3*sizeof(int) + sizeof(Mutex), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
        VERIFY_CONTRACT(errno == EEXIST, "Can\'t create shared memory\n");
        VERIFY_CONTRACT(
            (shmid = shmget(key, 3*sizeof(int) + sizeof(Mutex), 0)) >= 0, 
            "Can\'t find shared memory\n");
        new = 0;
    }

    VERIFY_CONTRACT((array = (int *)shmat(shmid, NULL, 0)) != (int *)(-1), "Can't attach shared memory\n");

    Mutex* mtx = (Mutex*)&array[3];
    if (new) {
        mutex_init(mtx);
        array[0] =    0;
        array[1] =    1;
        array[2] =    1;
        printf("Program 1 was spawn %d times, program 2 - %d times, total - %d times\n",
        array[0], array[1], array[2]);
    } else {
        mutex_lock(mtx, 1);
        array[1] += 1;
        for(i=0; i < 2000000000L; i++);
        array[2] += 1;

        printf("Program 1 was spawn %d times, program 2 - %d times, total - %d times\n",
        array[0], array[1], array[2]);
        mutex_unlock(mtx, 1);
    }


    VERIFY_CONTRACT(shmdt(array) >= 0, "Can't detach shared memory\n");

    return 0;
}


void  mutex_init(Mutex* mtx) {
    mtx->want_[0] = false;
    mtx->want_[1] = false;
}

void    mutex_lock(Mutex *mtx, int thrd) {
    VERIFY_CONTRACT((thrd == 1) || (thrd == 0), "Invalid thread id for Peterson mutex. Expected 0 or 1.\n");
    int     other = 1 - thrd;

    mtx->want_[thrd] = true;
    mtx->victim_ = thrd;


    while (mtx->want_[other] && (thrd == mtx->victim_)) {
        /* Backoff */
    }
}

void    mutex_unlock(Mutex *mtx, int thrd) {
    VERIFY_CONTRACT((thrd == 1) || (thrd == 0), "Invalid thread id for Peterson mutex. Expected 0 or 1.\n");
    mtx->want_[thrd] = false;
}

