#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <mpi.h>
#include <debug.h>
#include <print.h>
#include <rank_sort.h>

#define MAX_ELEM              (100000)

#ifndef PROG_NAME
#define PROG_NAME             (__FILE__)
#endif

void
print_usage (void)
{
	printf("Usage: "PROG_NAME" FILE SIZE\n");
	printf("\n\tFILE        File containing vector data\n");
	printf("\tSIZE        Size of vector (Maximum size: %d)\n", MAX_ELEM);
}

static void
do_master_stuff (int slaves, char *file, int elements)
{
	FILE *fd;
	int *readv;
	int *sortv;
	int i;
	int source;
	int ja_feito; // até que numero foi feito
	int cont_pos = 1; // variavel de posicao para colocar no vetor final
	int ja_recebido = array; // nao serve para nada ainda, pode apagar
	int vezes = array / valores_a_ordenar; // quntas vezes deve ser executado o loop
	int passa = vezes; // variavel para controlar quanta vezes ele deve passar pelo loop

	fd = fopen(file, "r");
	if (!fd) {
		print_errno("fopen() failed!");
	}

	sortv = calloc(elements, sizeof(*sortv));
	readv = calloc(elements, sizeof(*readv));
	if (fread(readv, sizeof(*readv), sizeof(readv), fd) < 0) {
		print_errno("fread() failed!");
	}
	fclose(fd);

	gettimeofday(&tvi, NULL);
	/**************************************************************
	Manda Np-1 tarefas
	***************************************************************/
	for (i = 1; i <= slaves; i++) {
		inicio = (valores_a_ordenar * cont) + 1; // inicio do pedaco
		MPI_Send(&inicio, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
		fim =  valores_a_ordenar * (cont + 1); // fim do pedaco
		MPI_Send(&fim, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
		cont++;
	}

	ja_feito = (valores_a_ordenar * (cont + 1)); // diz quantos já foram enviados pelo mestre

	/**************************************************************
	Loop do Mestre
	***************************************************************/
	while (vezes != 0) { // para poder receber de todos os escravos
		MPI_Recv(slave_vector, valores_a_ordenar, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
		source = status.MPI_SOURCE; // recebeu deste escravo
		vezes = vezes - 1; // decrementa valor
		/******************************************************
		Recebe do escravo e coloca no vetor
		*******************************************************/
		for (j = 0, i = (cont_pos-1); i < cont_pos + (valores_a_ordenar-1); i++, j++) { // receber vetor do escravo
			vector[i] = slave_vector[j];
		}
		if (cont > slaves) { // para poder ter recebeido de todos os escravos do FOR antes do WHILE 
			merge(vector, 0, cont_pos-1, cont_pos+valores_a_ordenar-1); // ordena
		}
		cont_pos = (valores_a_ordenar * ((cont+1)-slaves) + 1); // aumenta posicao do vetor final	
		if ((ja_feito) < array) { // ainda tem que gerar elementos
			inicio = (valores_a_ordenar * cont) + 1; // inicio do pedaco
			MPI_Send(&inicio, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
			fim =  (valores_a_ordenar * (cont + 1)); // fim do pedaco
			MPI_Send(&fim, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
		}

		if (cont < passa + slaves) { // para não passar o tamanho do vetor
			cont++;
		}
		ja_feito = (valores_a_ordenar * (cont)); // ataualiza valores já enviados
	}

	inicio = -1;

	for (i = 1; i <= slaves; i++) {
		// envia valor para encerrar escravo
		MPI_Send(&inicio, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
	}

	gettimeofday(&tvf, NULL);

	print_time(&tvi, &tvf);
}

static void
do_slave_stuff (void)
{
	rank_sort(readv, sortv, size);
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

	if (!pid) {
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

