// main.c
//
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this
// notice you can do whatever you want with this stuff. If we meet some
// day, and you think this stuff is worth it, you can buy me a beer in
// return.
//
// Initial version by Filipe Utzig <filipeutzig@gmail.com> on 3/26/15.
//
// The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
// "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in
// this document are to be interpreted as described in RFC 2119.
//
// Yet another sort app using Insertion Sort algorithm
// (Parallel MPI implementation)
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <utils.h>
#include <insertion_sort.h>
#include <mpi.h>

#define MAX_ELEM                                    (100000)
#define MPI_TAG                                     (0)
#define MPI_TERMINATE                               (-1)
#define FILENAME                                    "sorted_vector.txt"

void
print_usage (void)
{
	printf("Usage: %s FILE SIZE\n", PROG_NAME);
	printf("\tFILE File containing vector data\n");
	printf("\tSIZE Size of vector (Maximum size: %d)\n", MAX_ELEM);
}

static inline void
divide (int *recv, size_t recv_size, size_t send_size, int id)
{
	int next;
	int *sendv = &recv[recv_size - send_size];

	// CHECK THIS CALCULE
	nlog = ntotal / send_size;
	next = log (nlog) / log (2);
	next = (rank+(pow (2,next)));

	MPI_Send(sendv, send_size, MPI_INT, next, MPI_TAG, MPI_COMM_WORLD);
}

static inline void
conquer (int *recv, size_t sent_size, size_t sort_size, int id);
{
	int count;
	size_t received = 0;
	size_t ordered = sort_size;
	MPI_Status status;

	while (received != sent_size) {
		MPI_Recv(&recv[ordered], sent_size, MPI_INT, MPI_ANY_SOURCE,
		    MPI_TAG, MPI_COMM_WORLD, &status);
		MPI_Get_elements(&status, MPI_INT, &count);
		received += count;
		merge_vector(recv, 0, ordered, received + sort_size);
		ordered += count;
	}
}

static inline void
divide_and_conquer (int *recv, const size_t recv_size,
    const size_t conquer_size, int id)
{
	int has_divided = 0;
	int *sort;
	size_t my_size = recv_size;
	size_t send_size;
	size_t sent = 0;

	while (my_size > conquer_size) {
		send_size = my_size / 2;
		my_size -= send_size;
		divide(recv, recv_size, send_size, id);
		sent += send_size;
		has_divided = 1;
	}

	merge_sort(recv, 0, my_size);

	if (has_divided) {
		conquer(recv, sent, my_size, id);
	}

	return (sortv);
}

int
main (int argc, const char **argv)
{
	int id;
	int jobs;
	int *readv;
	int *sortv;
	int *recv;
	size_t i;
	size_t size;
	size_t recv_size;
	size_t conquer_size;
	MPI_Status status;

	prgname = argv[0];

	if (argc != 3) {
		print_usage();
		exit(1);
	}

	size = (size_t) atoi(argv[2]);
	if (size < 1 || size > MAX_ELEM) {
		print_error("Invalid number of elements: %zu!", size);
		exit(1);
	}

	MPI_Init(&argc, (char ***) &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &jobs);

	readv = calloc(size, sizeof(*readv));
	conquer_size = size / jobs;

	if (!id) {
		fd = fopen(file, "r");
		if (!fd) {
			print_errno("fopen() failed!");
			exit(1);
		}
		print_debug("conquer=%zu elements=%zu jobs=%d", conquer_size,
		    size, jobs);
		for (i = 0; i < size; i++) {
			ret = fscanf(fd, "%d", &readv[i]);
			if (ret < 0) {
				print_errno("fscanf() failed!");
				exit(ret);
			}
		}
		fclose(fd);

		gettimeofday(&ti, NULL);
		divide_and_conquer(readv, size, conquer_size, id);
		gettimeofday(&tf, NULL);
		print_time(begin, end);
		SAVE_RESULTS(RESULTS_WRITE, readv, size);
	} else {
		MPI_Recv(readv, size, MPI_INT, MPI_ANY_SOURCE, MPI_TAG,
		    MPI_COMM_WORLD, &status);
		MPI_Get_elements(&status, MPI_INT, &recv_size);
		divide_and_conquer(readv, recv_size, conquer_size, id);
		MPI_Send(readv, recv_size, MPI_INT, status.MPI_SOURCE,
		    MPI_TAG, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	free(readv);

	return (0);
}

