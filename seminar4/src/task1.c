#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <unistd.h>

#define VERIFY_CONTRACT(contract, message) \
if (!(contract)) { \
	printf(message); \
	exit(1); \
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		printf("No argument provided. Expected positive real number.\n");
		return 0;
	}

	if (argc > 2) {
		printf("Too many arguments provided. Expected positive real number.\n");
		return 0;
	}

	double a;
	int correct = sscanf(argv[1], "%lf", &a);
	VERIFY_CONTRACT(correct == 1, "ERROR: incorrect input!\n");
	VERIFY_CONTRACT(a >= 0.0, "ERROR: expected positive value\n");

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