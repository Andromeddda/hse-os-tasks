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


/**
    Below follows the text of the function
    that will be associated with the 2nd thread.

    Parameter dummy is used only for type compatibility.
    For the same reason, the function returns void *.
**/
void *mythread(void *dummy)
{
    (void)dummy;
    pthread_t mythid = pthread_self();

    a = a + 1;
    printf("Thread %lu, Calculation result = %d\n", mythid, a);
    return NULL;
}

/**
    The main() function is also an associated function of the main thread.
**/
int main()
{
    pthread_t thid1, thid2, mythid;
    int             result;


    // Create new thread
    result = pthread_create (&thid1, (pthread_attr_t *)NULL, mythread, NULL);
    VERIFY_CONTRACT(result == 0, "Error on thread create, return value = %d\n", result);
    printf("Thread created, thid = %lu\n", thid1);

    // Create new thread
    result = pthread_create (&thid2, (pthread_attr_t *)NULL, mythread, NULL);
    VERIFY_CONTRACT(result == 0, "Error on thread create, return value = %d\n", result);
    printf("Thread created, thid = %lu\n", thid2);

    mythid = pthread_self();

    a = a+1;

    printf("Thread %lu, Calculation result = %d\n", mythid, a);

    // Wait for the threads to finish
    pthread_join(thid1, (void **)NULL);
    pthread_join(thid2, (void **)NULL);


    return 0;
}
