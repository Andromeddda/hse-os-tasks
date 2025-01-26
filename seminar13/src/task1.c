#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define VERIFY_CONTRACT(contract, format, ...) \
if (!(contract)) { \
    printf(format, __VA_ARGS__); \
    exit(-1); \
}

// The variable a is global static for the entire program,
// so it will be shared by both threads of execution.
int a = 0;

void* mythread(void *dummy)
{
    (void)dummy;

    for (int i = 0; i < 10000000; i++)
        a++;
    return NULL;
}

/**
    The main() function is also an associated function of the main thread.
**/
int main()
{
    pthread_t thid1;
    int       result;


    // Create new thread
    result = pthread_create (&thid1, (pthread_attr_t *)NULL, mythread, NULL);
    VERIFY_CONTRACT(result == 0, "Error on thread create, return value = %d\n", result);

    // call function in main thread
    mythread(NULL);

    // Wait for the thread to finish
    pthread_join(thid1, (void **)NULL);

    printf("Calculation result = %d\n", a);
    return 0;
}
