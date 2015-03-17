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

static void
do_master_stuff (char *file, int elements)
{
	FILE *fd;
	int *readv;
	int *sortv;
	int escravo; // utilizada nos loops
	int ja_feito; // até que numero foi feito
	int cont_pos = 1; // variavel de posicao para colocar no vetor final
	int ja_recebido = array; // nao serve para nada ainda, pode apagar
	int vezes = array / valores_a_ordenar; // quntas vezes deve ser executado o loop
	int passa = vezes; // variavel para controlar quanta vezes ele deve passar pelo loop

	fd = fopen(file, "r");
	if (!fd) {
		perror("Invalid filename!");
	}

	sortv = calloc(elements, sizeof(*sortv));
	readv = calloc(elements, sizeof(*readv));
	if (fread(readv, sizeof(*readv), sizeof(readv), fd) < 0) {
		perror("fread() error!");
	}
	fclose(fd);

	gettimeofday(&tvi, NULL);
	/**************************************************************
	Manda Np-1 tarefas
	***************************************************************/
	for (escravo = 1; escravo < np; escravo++) {
		inicio = (valores_a_ordenar * cont) + 1; // inicio do pedaco
		MPI_Send(&inicio, 1, MPI_INT, escravo, tag, MPI_COMM_WORLD);
		fim =  valores_a_ordenar * (cont + 1); // fim do pedaco
		MPI_Send(&fim, 1, MPI_INT, escravo, tag, MPI_COMM_WORLD);
		cont++;
	}

	ja_feito = (valores_a_ordenar * (cont + 1)); // diz quantos já foram enviados pelo mestre

	/**************************************************************
	Loop do Mestre
	***************************************************************/
	while (vezes != 0) { // para poder receber de todos os escravos
		MPI_Recv(slave_vector, valores_a_ordenar, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
		escravo = status.MPI_SOURCE; // recebeu deste escravo
		vezes = vezes - 1; // decrementa valor
		/******************************************************
		Recebe do escravo e coloca no vetor
		*******************************************************/
		for (j = 0, i = (cont_pos-1); i < cont_pos + (valores_a_ordenar-1); i++, j++) { // receber vetor do escravo
			vector[i] = slave_vector[j];
		}
		if (cont > (np-1)) { // para poder ter recebeido de todos os escravos do FOR antes do WHILE 
			merge(vector, 0, cont_pos-1, cont_pos+valores_a_ordenar-1); // ordena
		}
		cont_pos = (valores_a_ordenar * ((cont+1)-(np-1)) + 1); // aumenta posicao do vetor final	
		if ((ja_feito) < array) { // ainda tem que gerar elementos
			inicio = (valores_a_ordenar * cont) + 1; // inicio do pedaco
			MPI_Send(&inicio, 1, MPI_INT, escravo, tag, MPI_COMM_WORLD);
			fim =  (valores_a_ordenar * (cont + 1)); // fim do pedaco
			MPI_Send(&fim, 1, MPI_INT, escravo, tag, MPI_COMM_WORLD);
		}

		if (cont < passa+(np-1)) { // para não passar o tamanho do vetor 
			cont++;
		}
		ja_feito = (valores_a_ordenar * (cont)); // ataualiza valores já enviados
	}

	inicio = -1;

	for (escravo = 1; escravo < np; escravo++) {
		// envia valor para encerrar escravo
		MPI_Send(&inicio, 1, MPI_INT, escravo, tag, MPI_COMM_WORLD);
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
		perror("Invalid argument number!");
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &jobs);

	if (!pid) {
		int size = atoi(argv[1]);
		if (size < 1 || size >= MAX_ELEM) {
			perror("Invalid number of elements!");
		}

		do_master_stuff(argv[0], size);
	} else {
		do_slave_stuff();
	}

	MPI_Finalize();

	return (0);
}
