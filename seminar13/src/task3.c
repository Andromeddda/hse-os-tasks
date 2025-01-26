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

pthread_mutex_t a_guard = PTHREAD_MUTEX_INITIALIZER;

void* mythread(void *dummy)
{
    (void)dummy;

    for (int i = 0; i < 10000000; i++) {
        VERIFY_CONTRACT(pthread_mutex_lock(&a_guard) == 0, "Can\'t lock mutex\n");
        a++;
        VERIFY_CONTRACT(pthread_mutex_unlock(&a_guard) == 0, "Can\'t unlock mutex\n");
    }
    return NULL;
}

int main()
{
    pthread_t thid1;
    int       result;

    // Create new thread
    result = pthread_create (&thid1, (pthread_attr_t *)NULL, mythread, NULL);
    VERIFY_CONTRACT_F(result == 0, "Error on thread create, return value = %d\n", result);

    // call function in main thread
    mythread(NULL);

    // Wait for the thread to finish
    pthread_join(thid1, (void **)NULL);

    printf("Calculation result = %d\n", a);
    return 0;
}
