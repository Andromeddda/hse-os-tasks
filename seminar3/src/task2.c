#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {

	pid_t id = fork();

	if (id == -1) {
		printf("ERROR: bad fork\n");
		exit(1);
	}
	else if (id == 0) {
		printf("FORK PARENT: pid == %d, ppid == %d\n", getpid(), getppid());
		exit(0);

	}
	else {
		printf("FORK CHILD: pid == %d, ppid == %d\n", getpid(), getppid());
		exit(0);
	}
	
	return 0;
}
