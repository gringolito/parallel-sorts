// main.c
//
// Copyright (c) 2015 Filipe Utzig. All rights reserved.
//
// Initial version by Filipe Utzig <filipeutzig@gmail.com> on 3/24/15.
//
// The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
// "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in
// this document are to be interpreted as described in RFC 2119.
//
// Yet another sort app using Insertion Sort algorithm
// (Sequential implementation)
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <utils.h>
#include <insertion_sort.h>

int
main (int argc, const char **argv)
{
	FILE *fd;
	int i;
	int x;
	int j;
	int valor;
	int aux;
	int vector[vetorord];
	int temp[vetorord];
	struct timeval begin;
	struct timeval end;

	fd = fopen(argv[1],"r");
	for(i = 0; i < vetorord; i++) {
		fscanf(fd, "%d", &vector[i]);
		temp[i] = vector[i];
	}
	fclose(fd);

	gettimeofday(&begin, NULL);
	insertion_sort(readv, sortv, size);
	gettimeofday(&end, NULL);

	print_time(begin, end);

	arq = fopen ("sorted_vector.txt", "w");
	for(i = 0; i < vetorord; i++) {
		fprintf(arq, "%d\n", vector[i]);
	}
	fflush(arq);
	fclose(arq);

	return (0);
}
