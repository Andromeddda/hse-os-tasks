#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
	printf("process id:\t%d\n", getpid());
	printf("parent process id:\t%d\n", getppid());
	return 0;
}
