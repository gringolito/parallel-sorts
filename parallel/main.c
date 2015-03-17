#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <mpi.h>

#define MAX_ELEM              (100000)

void
print_time (struct timeval initial, struct timeval final)
{
	int secs;
	int msecs;
	int usecs;

	msecs = (int) (final.tv_usec - initial.tv_usec) / 1000;
	usecs = (int) (final.tv_usec - initial.tv_usec) % 1000;
	secs = (int) (final.tv_sec - initial.tv_sec);

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

static inline int
merge_vector (int *vector, int begin, int mid, int end)
{
	int -ib = begin;
	int im = mid;
	int j;
	int size = end - begin;
	int *tmp;

	tmp = calloc(size, sizeof(*tmp));
	if (!tmp) {
		return (-1);
	}

	for (j = 0; j < size; j++) {
		if (ib < mid && (im >= end || vector[ib] <= vector[im])) {
			tmp[j] = vector[ib];
			ib = ib +1;
		} else {
			tmp[j] = vector[im];
			im = im + 1;
		}
	}

	for (j = 0, ib = begin; ib < end; j++, ib++) {
		vector[ib] = tmp[j];
	}

	free(tmp);
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
	int pid;
	int jobs;

	if (argc != 3) {
		perror("Invalid argument number!");
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &jobs);

	if (!pid) {
		int size;
		int readv[MAX_ELEM];
		int sortv[MAX_ELEM];
		FILE *fd;

		size = atoi(argv[1]);
		if (size < 1 || size >= MAX_ELEM) {
			perror("Invalid number of elements!");
		}

		fd = (argv[0], "r");
		if (!fd) {
			perror("Invalid filename!");
		}

		memset(readv, 0, sizeof(readv)*sizeof(*readv));
		if (fread(readv, sizeof(*readv), sizeof(readv), fd) < 0) {
			perror("fread() error!");
		}
		fclose(fd);

		gettimeofday(&tvi, NULL);
		/*
		 * TODO SOMETHING
		 */
		gettimeofday(&tvf, NULL);

		print_time(&tvi, &tvf);
	} else {
		rank_sort(readv, sortv, size);
	}

	MPI_Finalize();

	return (0);
}
