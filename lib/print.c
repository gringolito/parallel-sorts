#include <stdio.h>
#include <time.h>
#include <sys/time.h>

void
print_time (struct timeval init, struct timeval end)
{
	int secs;
	int msecs;
	int usecs;

	msecs = (int) (end.tv_usec - init.tv_usec) / 1000;
	usecs = (int) (end.tv_usec - init.tv_usec) % 1000;
	secs = (int) (end.tv_sec - init.tv_sec);

	if (msecs < 0) {
		secs--;
		msecs += 1000;
	}

	if (usecs < 0) {
		msecs--;
		usecs += 1000;
	}

	printf("Elasped Time: %ds:%dms:%dus\n", secs, msecs, usecs);
}

