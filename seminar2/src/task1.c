#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
	printf("user id:\t%d\ngroup id:\t%d\n", getuid(), getgid());
	return 0;
}
