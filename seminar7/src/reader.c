#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define VERIFY_CONTRACT(contract, msg) \
if (!(contract)) { \
    printf(msg); \
    exit(-1); \
}

const size_t buf_size = 1;

int main(void)
{
    char      *shared_buf;      // Pointer to shared memory
    int       shmid;            // IPC descriptor for the shared memory
    char      pathname[] = "src/writer.c"; // The file name used to generate the key.

    //
    // Generate an IPC key from the file name src/writer.c in the current directory
    // and the instance number of the shared memory 0.
    //
    key_t     key;
    VERIFY_CONTRACT((key = ftok(pathname,0)) >= 0, "Can\'t generate key\n");

    //
    // Trying to create a shared memory exclusively for the generated key, that is,
    // if it already exists for this key, the system call will return a negative value.
    // The memory size is defined as the size of an array of three integer variables,
    // access rights 0666 - reading and writing are allowed for everyone.
    //
    if ((shmid = shmget(key, 2*sizeof(char), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
        VERIFY_CONTRACT(errno == EEXIST, "Can\'t create shared memory\n");
        VERIFY_CONTRACT(
            (shmid = shmget(key, 2*sizeof(char), 0)) >= 0,
            "Can\'t find shared memory\n");
    }

    //
    // Try to map shared memory
    //
    VERIFY_CONTRACT(
        (shared_buf = (char*)shmat(shmid, NULL, 0)) != (char*)(-1),
        "Can't attach shared memory\n");

    ssize_t size = 1;
    do {
        VERIFY_CONTRACT(size > 0, "Error while writing to standart output\n");

        // set the flag to writing first
        shared_buf[1] = 'w';

        // wait until writer sets the flag to reading
        while (shared_buf[1] != 'r' && shared_buf[1] != 'E')
        { /* Backoff */}

    // read shared memory and write it to standart output
    } while ((shared_buf[0] != EOF) && shared_buf[1] != 'E' && (size = write(1, shared_buf, 1)));
    
    // set the flag to exit
    shared_buf[1] = 'E';
    
    VERIFY_CONTRACT(shmdt(shared_buf) >= 0, "Can't detach shared memory\n");
    return 0;
}
