#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <debug.h>
#include <print.h>
#include <rank_sort.h>
#include <mpi.h>

#define MAX_ELEM              (100000)
#define EVER                  (;;)
#define MPI_MASTER            (0)
#define MPI_TAG               (0)

#ifndef PROG_NAME
#define PROG_NAME             (__FILE__)
#endif

void
print_usage (void)
{
	printf("Usage: %s FILE SIZE\n", PROG_NAME);
	printf("\n\tFILE        File containing vector data\n");
	printf("\tSIZE        Size of vector (Maximum size: %d)\n", MAX_ELEM);
}

static void
do_master_stuff (int slaves, char *file, int elements)
{
	FILE *fd;
	int i;
	int slice;
	int begin;
	int last;
	int source;
	int *readv;
	int *sortv;
	struct timeval begin;
	struct timeval end;
	MPI_Status status;
	int cont_pos = 1; // variavel de posicao para colocar no vetor final
	int vezes = elements / slice; // quntas vezes deve ser executado o loop
	int passa = vezes; // variavel para controlar quanta vezes ele deve passar pelo loop

	fd = fopen(file, "r");
	if (!fd) {
		print_errno("fopen() failed!");
	}

	sortv = calloc(elements, sizeof(*sortv));
	readv = calloc(elements, sizeof(*readv));
	if (fread(readv, elements, sizeof(readv), fd) < 0) {
		print_errno("fread() failed!");
	}
	fclose(fd);

	// Number of elements ordered by a single slave by time
	slice = elements / (4 * slaves);

	gettimeofday(&begin, NULL);

	last = 0;
	for (i = 1; i <= slaves; i++) {
		MPI_Send(&slice, 1, MPI_INT, i, MPI_TAG, MPI_COMM_WORLD);
		MPI_Send(&readv[last], slice, MPI_INT, i, MPI_TAG, MPI_COMM_WORLD);
		last += slice;
	}

	while (vezes != 0) { // para poder receber de todos os escravos
		MPI_Recv(slave_vector, slice, MPI_INT, MPI_ANY_SOURCE, MPI_TAG, MPI_COMM_WORLD, &status);
		source = status.MPI_SOURCE; // recebeu deste escravo
		vezes = vezes - 1; // decrementa valor
		/******************************************************
		Recebe do escravo e coloca no vetor
		*******************************************************/
		for (j = 0, i = (cont_pos-1); i < cont_pos + (slice-1); i++, j++) { // receber vetor do escravo
			vector[i] = slave_vector[j];
		}
		if (cont > slaves) { // para poder ter recebeido de todos os escravos do FOR antes do WHILE 
			merge(vector, 0, cont_pos-1, cont_pos+slice-1); // ordena
		}
		cont_pos = (slice * ((cont+1)-slaves) + 1); // aumenta posicao do vetor final	
		if ((last) < elements) { // ainda tem que gerar elementos
			begin = (slice * cont) + 1; // inicio do pedaco
			MPI_Send(&begin, 1, MPI_INT, source, MPI_TAG, MPI_COMM_WORLD);
			fim =  (slice * (cont + 1)); // fim do pedaco
			MPI_Send(&fim, 1, MPI_INT, source, MPI_TAG, MPI_COMM_WORLD);
		}

		if (cont < passa + slaves) { // para não passar o tamanho do vetor
			cont++;
		}
		last = (slice * (cont)); // ataualiza valores já enviados
	}

	begin = -1;

	for (i = 1; i <= slaves; i++) {
		// envia valor para encerrar escravo
		MPI_Send(&begin, 1, MPI_INT, i, MPI_TAG, MPI_COMM_WORLD);
	}

	gettimeofday(&end, NULL);

	print_time(begin, end);
}

static void
do_slave_stuff (void)
{
	int size;
	int received;
	int *recv;
	int *sortv;
	MPI_Status status;

	MPI_Recv(&size, 1, MPI_INT, MPI_MASTER, MPI_TAG, MPI_COMM_WORLD,
	    &status);

	recv = calloc(size, sizeof(*recv));
	sortv = calloc(size, sizeof(*sortv));

	for (EVER) { // fica trabalhando enquanto o mestre não terminar
		MPI_Recv(recv, size, MPI_INT, MPI_MASTER, MPI_TAG,
		    MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &received);
		if (received == 1 && recv[0] == -1) {
			break;
		} else {
			rank_sort(recv, sortv, received);
			MPI_Send(sortv, received, MPI_INT, MPI_MASTER, MPI_TAG,
			    MPI_COMM_WORLD);
	}

	free(recv);
	free(sortv);
}

int
main (int argc, char **argv)
{
	int pid;
	int jobs;

	if (argc != 3) {
		print_usage();
		exit (1);
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &jobs);

	if (pid == MPI_MASTER) {
		int size = atoi(argv[1]);
		if (size < 1 || size >= MAX_ELEM) {
			print_error("Invalid number of elements: %d!", size);
			exit (1);
		}

		do_master_stuff(jobs - 1, argv[0], size);
	} else {
		do_slave_stuff(pid);
	}

	MPI_Finalize();

	return (0);
}

