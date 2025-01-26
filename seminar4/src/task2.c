#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define VERIFY_CONTRACT(contract, message) \
if (!(contract)) { \
	printf(message); \
	exit(1); \
}

char* custom_getenv(char *envp[], char* envname);

int main(int argc, char *argv[], char *envp[]) {
	// get rid of unused variable warning
	(void)argc;
	(void)argv;

	// Get environmental variable
	char* input = custom_getenv(envp, "TASK2INPUT");
	if (!input)
		printf("No environmental variable provided as TASK2INPUT.\n");

	// Read double from string
	double a;
	int correct = sscanf(input, "%lf", &a);
	VERIFY_CONTRACT(correct == 1 && a >= 0.0, 
		"ERROR: environmental TASK2ENV expected to be a positive real number\n");

	// Calculations
	double epsilon = 0.000001;
	double xprev;
	double xcur = 1.0;

	do {
		xprev = xcur;
		xcur = 0.5 * (xprev + a / xprev);
	} while (fabs(xcur - xprev) >= epsilon);

	printf("%lf\n", xcur);
	return 0;
}

char* custom_getenv(char *envp[], char* envname) {
	for (int i = 0; envp[i] != NULL; i++) {
		int j = 0;
		// compare 'til prefixes are equal
		while (envp[i][j] != '\0'
			&& envp[i][j] != '='
			&& envname[j] != '\0' 
			&& envp[i][j] == envname[j])
			j++;

		// if while achieved the end of envname, variable found 
		if (envname[j] == '\0')
			return &envp[i][j + 1];
	}
	return NULL;	
}
