#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define VERIFY_CONTRACT(contract, format) \
if (!(contract)) { \
    printf(format); \
    exit(-1); \
}

#define VERIFY_CONTRACT_F(contract, format, ...) \
if (!(contract)) { \
    printf(format, __VA_ARGS__); \
    exit(-1); \
}

// The variable a is global static for the entire program,
// so it will be shared by both threads of execution.
int a = 0;


struct peterson_lock {
    bool    want_[2];
    int      victim_;
};

typedef struct peterson_lock Lock;

Lock a_guard;

void    peterson_init(Lock *lock);
void    peterson_lock(Lock *lock, int thrd);
void    peterson_unlock(Lock *lock, int thrd);


void* mythread(void *info)
{
    int thrd = *((int*)info);

    for (int i = 0; i < 10000000; i++) {
        peterson_lock(&a_guard, thrd);
        a++;
        peterson_unlock(&a_guard, thrd);
    }
    return NULL;
}


int main(void)
{
    pthread_t thid1;
    int       result;

    peterson_init(&a_guard);

    int us = 0;
    int child = 1;

    // Create new thread
    result = pthread_create (&thid1, (pthread_attr_t *)NULL, mythread, (void *)&child);
    VERIFY_CONTRACT_F(result == 0, "Error on thread create, return value = %d\n", result);

    // call function in main thread
    mythread((void *)&us);

    // Wait for the thread to finish
    pthread_join(thid1, (void **)NULL);

    printf("Calculation result = %d\n", a);
    return 0;
}

void    peterson_init(Lock *lock) {
    lock->want_[1] = false;
    lock->want_[2] = false;
}

void    peterson_lock(Lock *lock, int thrd) {
    VERIFY_CONTRACT((thrd == 1) || (thrd == 0), "Invalid thread id for Peterson. Expected 0 or 1.\n");
    lock->want_[thrd] = true;
    lock->victim_ = thrd;

    while (lock->want_[1 - thrd] && (thrd == lock->victim_)) {  /* Backoff */ }
}

void    peterson_unlock(Lock *lock, int thrd) {
    VERIFY_CONTRACT((thrd == 1) || (thrd == 0), "Invalid thread id for Peterson mutex. Expected 0 or 1.\n");
    lock->want_[thrd] = false;
}

