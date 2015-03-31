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

/* Função que imprime o vetor resultado de cada etapa */
void
Sequencia (int vector[], int tam) { 
	FILE *arq;
	int k;
	arq = fopen ("/home/felipealves/Downloads/MPI/B/Paralelo/operacao.txt", "a+");
	for (k = 0; k < tam; k++) // recebe todos os valores da etapa
		fprintf(arq, "%d\n", vector[k]); // ecreve no arquivo
	fflush(arq);
	fclose(arq);
}

static void
do_first_stage (const char *file, int size, int jobs)
{
	int i;
	int ret;
	int slice = size / jobs;
	int readv[size];
	int sortv[slice + 1];
	struct timeval begin;
	struct timeval end;
	FILE *fd;

	fd = fopen(argv[1], "r");
	if (!fd) {
		print_errno("fopen() failed!");
		exit(ret);
	}

	readv = calloc(size, sizeof(*readv));
	sortv = calloc(size, sizeof(*sortv));
	for (i = 0; i < size; i++) {
		ret = fscanf(fd, "%d", &readv[i]);
		if (ret < 0) {
			print_errno("fscanf() failed!");
			exit(ret);
		}
	}
	fclose(fd);

	gettimeofday(&begin);

	valor = temp[j]; // lê o valor da posição
	Compara(rank, &valor, vector, j, tam); // Compara
	if (((j+1) == array) && np > 1) // caso percorreu todos os valores e existem mais processadores
		MPI_Send(&end, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD); // envia para o próximo que pode terminar
}

static void
do_pipeline (int id, int size, int jobs)
{
	MPI_Recv(&valor,1,MPI_INT,rank-1,tag,MPI_COMM_WORLD,&status); // recebe do anterior o valor da função Compara
	if (valor == -1){ // se for igual a 1
		j = array;  // pode terminar
		if (rank+1 < np) // se não é o último processador
			MPI_Send(&end, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD); // envia a mensagem de fim para o próximo 
	}  
	else // senão
		Compara(rank, &valor, vector, j, tam); // Compara valor recebido no MPI_Recv

}

int
main (int argc, const char **argv)
{
	int id;
	int jobs;


	prgname = argv[0];

	if (argc != 3) {
		print_usage();
		exit(1);
	}

	size = atoi(argv[2]);
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
	/******************************************************************
	   Fecha arquivo
	 *****************************************************************/
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&np);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	int tam = array / np; // é a variável que dirá o tamanho de cada etapa
	/*****************************************************************/
	if (rank == 0) { // caso seja a etapa 0
		remove("/home/felipealves/Downloads/MPI/B/Paralelo/operacao.txt"); // remove, caso existir o arquivo de saída
		gettimeofday(&ti, NULL); // tempo inicial
	}
	/*****************************************************************/
	for (j = 0; j < array; j++){ // percorre todos os valores
		if(rank == 0){ // caso seja a etapa 0
		}
		else { // todas as outras etapas
		}
	}
	/***************************************************************/
	MPI_Barrier(MPI_COMM_WORLD); /* Espera todos os processadores chegara até aqui */
	if(rank == 0){ // se for a etapa zero
		gettimeofday(&tf, NULL); // pega o tempo final
		imprime_tempo(ti, tf);  // imprime
	}
	/***************************************************************/
	MPI_Barrier(MPI_COMM_WORLD); // espera todos os processadores chegarem até aqui
	if(rank == 0){ // se for a etapa zero
		Sequencia(vector, tam); // imprime sua parte
		if (rank+1 < np) // senão for o último processador
			MPI_Send(&imprime, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD); // manda para o próximo que pode imprimir
	}
	else {
		MPI_Recv(&imprime,1,MPI_INT,rank-1,tag,MPI_COMM_WORLD,&status); // recebe do anterior que pode imprimir seus valores
		Sequencia(vector, tam); // imprime sua parte
		if (rank+1 < np) // senão for o último processador
			MPI_Send(&imprime, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD); // manda para o próximo que pode imprimir
	}
	MPI_Barrier(MPI_COMM_WORLD); // espera todos chegarem até aqui





	MPI_Finalize();

	return (0);
}
