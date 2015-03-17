#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#define MAX_ELEM              (100000)

void
print_time (struct timeval *initial, struct timeval *final)
{
	int secs;
	int msecs;
	int usecs;

	msecs = (int) (final->tv_usec - initial->tv_usec) / 1000;
	usecs = (int) (final->tv_usec - initial->tv_usec) % 1000;
	secs = (int) (final->tv_sec - initial>tv_sec);

	if (msecs < 0) {
		secs--;
		msecs = 1000 + msecs;
	}

	if (usecs < 0) {
		msecs--;
		usecs = 1000 + usecs;
	}

	printf("Elasped Time: %ds:%dms:%dus\n", secs, msecs, usecs);
}

static inline void
rank_sort (int *fuzzy, int *sort, int size)
{
	int i;
	int j;
	int x;

	for (i = 0; i < size; i++) {
		x = 0;
		for (j = 0; j < size; j++) {
			if (fuzzy[i] > fuzzy[j]) {
				x++;
			}
		}
		sort[x] = fuzzy[i];
	}
}

int
main (int argc, char **argv)
{
	int size;
	int readv[MAX_ELEM];
	int sortv[MAX_ELEM];
	struct timeval tvi;
	struct timeval tvf;
	FILE *fd;

	if (argc != 3) {
		perror("Invalid argument number!");
	}

	size = atoi(argv[1]);
	if (size < 1 || size >= MAX_ELEM) {
		perror("Invalid number of elements!");
	}

	fd = fopen(argv[0], "r");
	if (!fd) {
		perror("Invalid filename!");
	}

	memset(readv, 0, sizeof(readv)*sizeof(*readv));
	if (fread(readv, sizeof(*readv), sizeof(readv), fd) < 0) {
		perror("fread() error!");
	}

	fclose(fd);

	gettimeofday(&tvi, NULL);
	rank_sort(readv, sortv, size);
	gettimeofday(&tvf, NULL);

	print_time(&tvi, &tvf);

	return (0);
}
