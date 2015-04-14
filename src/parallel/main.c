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

static int
divide (void)
{
	// calcula o proximo
	nlog = ntotal / send_size;
	// divide o valor a enviar por dois (inicilizado com array)
	send_size = send_size / 2;
	// divide o vetor
	for (i = 0, j = 0, k = send_size; i < send_size; i++, j++, k++) {
		// parte mais baixa fica
		received_vector[j] = vector[i];
		// parte mais alta e enviada
		send_vector[j] = vector[k];
	}
	// calcula o proximo
	next = log (nlog) / log (2);
	next = (rank+(pow (2,next)));
	MPI_Send(send_vector, send_size, MPI_INT, next, MPI_TAG, MPI_COMM_WORLD);
}

	static int
conquer (void)
{
	// ordena sua parte do nodo 0
	Sort(received_vector,0, send_size);
	// enquanto não receber o numero de elementos do array original
	while (send_size != array) {
		// recebe dos filhos
		MPI_Recv(send_vector, send_size, MPI_INT, MPI_ANY_SOURCE,
		    tag, MPI_COMM_WORLD, &status);
		MPI_Get_elements(&status, MPI_INT, &received_size);
		old_size = send_size;
		send_size = send_size + received_size;
		// recebe as outras partes
		for (i = received_size, j = 0; i < send_size; i++, j++)
			received_vector[i] = send_vector[j];
		Merge(received_vector, 0, old_size, send_size);
	}
}

	static int
divide_and_conquer (void)
{
	while (send_size != tam) {
		divide();
	}
	conquer();

}

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
		print_error("Invalid number of elements: %zu!", size);
		exit(1);
	}

	MPI_Init(&argc, (char ***) &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &jobs);

	int tam = array / np;
	if (!id) {
		fd = fopen(file, "r");
		if (!fd) {
			print_errno("fopen() failed!");
			exit(1);
		}
		slice = size / jobs;
		print_debug("slice=%zu elements=%zu jobs=%d", slice, size, jobs);
		readv = calloc(size, sizeof(*readv));
		sortv = calloc(slice, sizeof(*sortv));
		for (i = 0; (size_t) i < size; i++) {
			ret = fscanf(fd, "%d", &readv[i]);
			if (ret < 0) {
				print_errno("fscanf() failed!");
				exit(ret);
			}
		}
		fclose(fd);

		gettimeofday(&ti, NULL);
		divide_and_conquer();
		gettimeofday(&tf, NULL);
		print_time(begin, end);
		SAVE_RESULTS(RESULTS_WRITE, sortv, slice);
	} else {
		MPI_Recv(received_vector, send_size, MPI_INT, MPI_ANY_SOURCE,
		    tag, MPI_COMM_WORLD, &status);
		MPI_Get_elements(&status, MPI_INT, &received_size);
		received_from = status.MPI_SOURCE;
		send_size = received_size;
		original_size = received_size;
		last_one = 1;
		divide_and_conquer();
	}







		// enquanto o que foi enviado nao e tamanho final de cada processador
		while(send_size != tam){
			last_one = 0;
			// calcula o proximo
			nlog = ntotal / send_size;
			send_size = send_size / 2;
			for (i = 0, j = 0, k = send_size; i < send_size;
			    i++, j++, k++)
				// parte mais alta e enviada
				send_vector[j] = received_vector[k];
			// calcula o proximo
			next = log (nlog) / log (2);
			next = (rank+(pow (2,next)));
			MPI_Send(send_vector, send_size, MPI_INT, next, tag,
			    MPI_COMM_WORLD);
		}
		Sort(received_vector,0, send_size);
		if (last_one == 1) {
			MPI_Send(received_vector, received_size, MPI_INT,
			    received_from, tag, MPI_COMM_WORLD);
		} else {
			while(send_size != original_size){
				MPI_Recv(send_vector, send_size, MPI_INT,
				    MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
				MPI_Get_elements(&status, MPI_INT, &received_size);
				old_size = send_size;
				send_size = send_size + received_size; 
				send_one = received_size;
				if (received_size == 1)
					// incrementa send_one para não duplicar em i = 1
					send_one++;
				for (i = send_one, j = 0; i < send_size; i++, j++)
					received_vector[i] = send_vector[j];
				Merge(received_vector, 0, old_size, send_size);
			}
			MPI_Send(received_vector, original_size, MPI_INT,
			    received_from, tag, MPI_COMM_WORLD);
		}
	}





	MPI_Finalize();

	return (0);
}

