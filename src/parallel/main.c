// main.c
//
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this
// notice you can do whatever you want with this stuff. If we meet some
// day, and you think this stuff is worth it, you can buy me a beer in
// return.
//
// Initial version by Filipe Utzig <filipeutzig@gmail.com> on 3/19/15.
//
// The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
// "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in
// this document are to be interpreted as described in RFC 2119.
//
// Main example file
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

/* Função que compara, armazena e envia (se for o caso) valores no vetor*/
void
compara (int id, int *valor, int vector[], int step, int tam)
{
	int flag = 0, // variável para indicar que houve inserção no vetor
	    aux, // variável auxiliar para receber valor do vetor
	    j, // utilizado nos loops
	    tag = 0, // utilizado na comunicação MPI
	    ultimo; // variável que armazena último valor do vetor
	if (step < tam){ // caso ainda não tenha preenchido todas as posições do vetor
		for (j = 0; j < step; j++) { // percorre até onde ele recebeu
			if (*valor < vector[j]){ // algoritmo pipeline
				aux = vector[j];
				vector[j] = *valor;
				*valor = aux;
			}
		}
		vector[step] = *valor;
	}
	else { // caso já tenha preenchido todas as posições do vetor, testando a necessidade de inseri-lo
		ultimo = vector[tam-1]; // obtém a última posição do vetor para o caso de houver troca
		for (j = 0; j < tam; j++) { // algoritmo pipeline
			if (*valor < vector[j]){
				flag = 1; // houve inserção de valores
				aux = vector[j];
				vector[j] = *valor;
				*valor = aux;
			}
		}
		if (flag == 1) // caso houve inserção
			MPI_Send(&ultimo,1,MPI_INT,id+1,tag,MPI_COMM_WORLD); // manda a variável ultimo para o próximo, que contém a última posição
		else // caso não houve inserção
			MPI_Send(valor,1,MPI_INT,id+1,tag,MPI_COMM_WORLD);  // manda o valor lido para o próximo           
	}
}

static void
do_first_stage (const char *file, size_t size, int jobs)
{
	int i;
	int ret;
	int slice;
	int *readv;
	int *sortv;
	struct timeval begin;
	struct timeval end;
	FILE *fd;
	MPI_Status status;

	fd = fopen(argv[1], "r");
	if (!fd) {
		print_errno("fopen() failed!");
		exit(ret);
	}

	slice = size / jobs;
	readv = calloc(size, sizeof(*readv));
	sortv = calloc(slice + 1, sizeof(*sortv));
	for (i = 0; i < size; i++) {
		ret = fscanf(fd, "%d", &readv[i]);
		if (ret < 0) {
			print_errno("fscanf() failed!");
			exit(ret);
		}
	}
	fclose(fd);

	gettimeofday(&begin);

	valor = temp[j];
	Compara(rank, &valor, vector, j, tam);
	// envia para o próximo que pode terminar
	MPI_Send(&end, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD);
}

static void
do_pipeline (int id, size_t size, int jobs)
{
	int slice;
	int elements;
	int recv;
	int last;
	int next;
	int *buf;
	FILE fd;
	MPI_Status status;

	if (id != jobs) {
		slice = size / jobs;
		elements = size - slice * id;
		last = id - 1;
	} else {
		// Last stage of pipeline takes the rest of elements
		slice = size % jobs;
		elements = slice;
		next = 0;
	}

	buf = calloc(slice + 1, sizeof(*buf));
	
	while (elements--) {
		MPI_Recv(&recv, 1, MPI_INT, last, MPI_TAG, MPI_COMM_WORLD,
		    &status);
		Compara(id, &recv, buf, j, tam);
	}

	recv = MPI_TERMINATE;
	MPI_Send(&recv, 1, MPI_INT, next, MPI_TAG, MPI_COMM_WORLD);
	MPI_Recv(&recv, 1, MPI_INT, last, MPI_TAG, MPI_COMM_WORLD, &status);

	fd = fopen(FILENAME, "a+");
	if (!fd) {
		print_errno("fopen() failed!");
		exit(1);
	}
	if (fprint_intvector(fd, buf, slice)) {
		print_error("fprint_intvector() failed!");
	}
	fflush(fd);
	fclose(fd);

	recv = MPI_TERMINATE;
	MPI_Send(&recv, 1, MPI_INT, next, MPI_TAG, MPI_COMM_WORLD);
}

#if 0
	int array = atoi(argv[1]); // array será o tamanho de elementos a ordenar
	if (array < 1)
		return (1);
	FILE *fp;
	int vector[array]; // vetor que vai ser o ordenado e utilizado pelas etapas
	int temp[array];
	int rank, // "id" de cada processador
	    np, // número de processadores
	    j, // utilizado nos loops
	    valor, // valor lido pela etapa 0
	    tag = 0, // utilizado na comunicação MPI
	    end = -1, // determina que etapa 0 leu todos os valores
	    imprime = 1; // pode imprimir (sem valor lógico, apenas demonstrativo)
	struct timeval ti, tf; // para imprimir tempo
	int tam = array / np; // é a variável que dirá o tamanho de cada etapa
#endif

int
main (int argc, const char **argv)
{
	int id;
	int jobs;
	size_t size;

	prgname = argv[0];

	if (argc != 3) {
		print_usage();
		exit(1);
	}

	size = (size_t) atoi(argv[2]);
	if (size < 1 || size > MAX_ELEM) {
		print_error("Invalid number of elements: %d!", size);
		exit (1);
	}

	MPI_Init(&argc, (char ***) &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &jobs);

	if (!id) {
		do_first_stage(argv[1], size, jobs);
	} else {
		do_pipeline(id, size, jobs);
	}

	MPI_Finalize();

	return (0);
}

