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
	int last;
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

	last = 0;
	for (i = 0; i < jobs; i++) {
		if (i == MPI_MASTER) {
			continue;
		}
		MPI_Send(&slice, 1, MPI_INT, i, MPI_TAG, MPI_COMM_WORLD);
		MPI_Send(&readv[last], slice, MPI_INT, i, MPI_TAG, MPI_COMM_WORLD);
		last += slice;
		remaining -= slice;
	}

	while (remaining) {
		MPI_Recv(sortv, slice, MPI_INT, MPI_ANY_SOURCE, MPI_TAG,
		    MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &ret);
		source = status.MPI_SOURCE;

// TODO VERIFICAR LOGICA DE MERGE
		// Recebe do escravo e coloca no vetor
		for (j = 0, i = (cont_pos-1); i < cont_pos + (slice-1); i++, j++) { // receber vetor do escravo
			vector[i] = slave_vector[j];
		}
		if (cont > slaves) { // para poder ter recebeido de todos os escravos do FOR antes do WHILE 
			merge(vector, 0, cont_pos-1, cont_pos+slice-1); // ordena
		}
		cont_pos = (slice * ((cont+1)-slaves) + 1); // aumenta posicao do vetor final	
// TODO FIM DA LOGICA DE MERGE

		slice = MIN(slice, remaining);
		MPI_Send(&readv[last], slice, MPI_INT, source, MPI_TAG,
		    MPI_COMM_WORLD);
		last += slice;
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

