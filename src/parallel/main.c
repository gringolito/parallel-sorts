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
	int to_send;
	int last_send;
	int last_recv;
	int source;
	int remaining;
	int readv[elements];
	int sortv[elements];
	FILE *fd;
	struct timeval begin;
	struct timeval end;
	MPI_Status status;

	fd = fopen(file, "r");
	if (!fd) {
		print_errno("fopen() failed!");
		exit(ret);
	}

	for (i = 0; i < elements; i++) {
		ret = fscanf(fd, "%d", &readv[i]);
		if (ret < 0) {
			print_errno("fscanf() failed!");
			exit(ret);
		}
	}
	fclose(fd);

	remaining = elements;

	// Maximum number of elements sorted by a single slave at a time
	slaves = jobs - 1;
	slice = elements / (4 * slaves);
	print_debug("elements %d slice %d slaves %d", elements, slice, slaves);

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
		print_debug("i %d remaining %d slice %d", i, remaining, slice);
	}

	last_recv = 0;
	while (last_recv != elements) {
		MPI_Recv(&sortv[last_recv], slice, MPI_INT, MPI_ANY_SOURCE,
		    MPI_TAG, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &ret);
		source = status.MPI_SOURCE;
		print_debug("remaining %d slice %d ret %d", remaining, slice, ret);
		merge_vector(sortv, 0, last_recv, last_recv + ret);
		last_recv += ret;
		if (remaining) {
			to_send = MIN(slice, remaining);
			MPI_Send(&readv[last_send], to_send, MPI_INT, source,
			    MPI_TAG, MPI_COMM_WORLD);
			last_send += to_send;
			remaining -= to_send;
		}
	}

	remaining = MPI_TERMINATE;
	for (i = 0; i < jobs; i++) {
		if (i == MPI_MASTER) {
			continue;
		}
		print_debug("i %d MPI_TERMINATE", i);
		MPI_Send(&remaining, 1, MPI_INT, i, MPI_TAG, MPI_COMM_WORLD);
	}

	gettimeofday(&end, NULL);

	print_time(begin, end);

	fd = fopen("sorted_vector.txt", "w");
	if (!fd) {
		print_errno("fopen() failed!");
		exit(ret);
	}

	for (i = 0; i < elements; i++) {
		fprintf(fd, "%d\n", sortv[i]);
	}
	fclose(fd);
	printf("The result can be found at file 'sorted_vector.txt'\n");
}

static void
do_slave_stuff (void)
{
	int size;
	int rec_size;
	MPI_Status status;

	MPI_Recv(&size, 1, MPI_INT, MPI_MASTER, MPI_TAG, MPI_COMM_WORLD,
	    &status);
	print_debug("size %d", size);
	int recv[size];
	int sortv[size];

	for EVER {
		MPI_Recv(recv, size, MPI_INT, MPI_MASTER, MPI_TAG,
		    MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &rec_size);
		print_debug("rec_size %d", rec_size);
		if (rec_size == 1 && recv[0] == MPI_TERMINATE) {
			print_debug("MPI_TERMINATE");
			break;
		}

		rank_sort(recv, sortv, rec_size);
		MPI_Send(sortv, rec_size, MPI_INT, MPI_MASTER, MPI_TAG,
		    MPI_COMM_WORLD);
	}
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

	MPI_Init(&argc, (char ***) &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &jobs);

	if (pid == MPI_MASTER) {
		int size = atoi(argv[2]);
		if (size < 1 || size > MAX_ELEM) {
			print_error("Invalid number of elements: %d!", size);
			exit (1);
		}

		do_master_stuff(jobs, argv[1], size);
	} else {
		do_slave_stuff();
	}

	MPI_Finalize();

	return (0);
}

