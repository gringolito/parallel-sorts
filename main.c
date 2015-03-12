#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <mpi.h>

#define MAX_ELEM              (100000)

int
main (int argc, char **argv)
{
	int pid;
	int size;
	FILE *fd;

	if (argc != 2) {
		perror("Invalid argument!");
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (!pid) {
		int readv[MAX_ELEM];
		int sortv[MAX_ELEM];

		fd = (argv[0], "r");
		if (!fd) {
			perror("Invalid filename!");
		}

		if (fread(readv, sizeof(*readv), sizeof(readv), fd) < 0) {
			perror("fread() error!");
		}
	} else {

	}

	MPI_Finalize();

	return (0);
}
