#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <unistd.h>

int main(void) {
	pid_t id = fork();

	if (id < 0) {
		printf("ERROR: bad fork\n");
		exit(1);
	}
	else if (id == 0) {
	    int exec_code = execl("/bin/ls", "/bin/ls", 0);

	    if (exec_code == -1)
		    printf("CHILD FORK: bad exec. Make sure you are in the 'os-shikalov-233-2' directory\n");
	}
	else {
	    printf("PARENT FORK: child fork is executing /bin/ls. No secrets.\n");
	    exit(0);
	}
	
	return 0;
}