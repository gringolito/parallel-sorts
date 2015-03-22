// main.c
//
// Copyright (c) 2015 Filipe Utzig. All rights reserved.
//
// Initial version by Filipe Utzig <filipeutzig@gmail.com> on 3/19/15.
//
// The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
// "SHOULD", "SHOULD NOT", "RECOMMENDED",  "MAY", and "OPTIONAL" in
// this document are to be interpreted as described in RFC 2119.
//
// Yet another sort app using rank_sort algothim (Parallel MPI implementation)
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <utils.h>
#include <rank_sort.h>
#include <mpi.h>

#define MAX_ELEM              (100000)
#define MPI_MASTER            (0)
#define MPI_TAG               (0)
#define MPI_TERMINATE         (-1)

void
print_usage (void)
{
	printf("Usage: %s FILE SIZE\n", PROG_NAME);
	printf("\tFILE        File containing vector data\n");
	printf("\tSIZE        Size of vector (Maximum size: %d)\n", MAX_ELEM);
}

static void
do_master_stuff (int jobs, const char *file, int elements)
{
	int i;
	int ret;
	int slaves;
	int slice;
	int last_send;
	int last_recv;
	int source;
	int remaining;
	int *readv;
	int *sortv;
	FILE *fd;
	struct timeval begin;
	struct timeval end;
	MPI_Status status;

	fd = fopen(file, "r");
	if (!fd) {
		print_errno("fopen() failed!");
		exit(ret);
	}

	sortv = calloc(elements, sizeof(*sortv));
	readv = calloc(elements, sizeof(*readv));
	ret = fread(readv, elements, sizeof(readv), fd);
	if (ret < 0) {
		print_errno("fread() failed!");
		exit(ret);
	}
	fclose(fd);

	remaining = elements;

	// Maximum number of elements sorted by a single slave at a time
	slaves = jobs - 1;
	slice = elements / (4 * slaves);

	gettimeofday(&begin, NULL);

	last_send = 0;
	for (i = 0; i < jobs; i++) {
		if (i == MPI_MASTER) {
			continue;
		}
		MPI_Send(&slice, 1, MPI_INT, i, MPI_TAG, MPI_COMM_WORLD);
		MPI_Send(&readv[last_send], slice, MPI_INT, i, MPI_TAG,
		    MPI_COMM_WORLD);
		last_send += slice;
		remaining -= slice;
	}

	last_recv = 0;
	while (remaining) {
		MPI_Recv(sortv[last_recv], slice, MPI_INT, MPI_ANY_SOURCE,
		    MPI_TAG, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &ret);
		source = status.MPI_SOURCE;
		merge(sortv, 0, last_recv, last_recv + ret);
		last_recv += ret;
		slice = MIN(slice, remaining);
		MPI_Send(&readv[last_send], slice, MPI_INT, source, MPI_TAG,
		    MPI_COMM_WORLD);
		last_send += slice;
	}

	remaining = MPI_TERMINATE;

	for (i = 0; i < jobs; i++) {
		if (i == MPI_MASTER) {
			continue;
		}
		MPI_Send(&remaining, 1, MPI_INT, i, MPI_TAG, MPI_COMM_WORLD);
	}

	gettimeofday(&end, NULL);

	print_time(begin, end);
}

static void
do_slave_stuff (int pid)
{
	int size;
	int rec_size;
	int *recv;
	int *sortv;
	MPI_Status status;

	MPI_Recv(&size, 1, MPI_INT, MPI_MASTER, MPI_TAG, MPI_COMM_WORLD,
	    &status);

	recv = calloc(size, sizeof(*recv));
	sortv = calloc(size, sizeof(*sortv));

	for (EVER) {
		MPI_Recv(recv, size, MPI_INT, MPI_MASTER, MPI_TAG,
		    MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &rec_size);
		if (rec_size == 1 && recv[0] == MPI_TERMINATE) {
			break;
		}

		rank_sort(recv, sortv, received);
		MPI_Send(sortv, rec_size, MPI_INT, MPI_MASTER, MPI_TAG,
		    MPI_COMM_WORLD);
	}

	free(recv);
	free(sortv);
}

int
main (int argc, const char **argv)
{
	int pid;
	int jobs;

	prgname = argv[0];

	if (argc != 3) {
		print_usage();
		exit (1);
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &jobs);

	if (pid == MPI_MASTER) {
		int size = atoi(argv[2]);
		if (size < 1 || size >= MAX_ELEM) {
			print_error("Invalid number of elements: %d!", size);
			exit (1);
		}

		do_master_stuff(jobs, argv[1], size);
	} else {
		do_slave_stuff(pid);
	}

	MPI_Finalize();

	return (0);
}

