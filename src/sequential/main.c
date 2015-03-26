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
#include <errno.h>
#include <sys/time.h>
#include <utils.h>
#include <insertion_sort.h>

#define MAX_ELEM                                    (100000)

void
print_usage (void)
{
	printf("Usage: %s FILE SIZE\n", PROG_NAME);
	printf("\tFILE File containing vector data\n");
	printf("\tSIZE Size of vector (Maximum size: %d)\n", MAX_ELEM);
}

int
main (int argc, const char **argv)
{
	int i;
	int ret;
	int size;
	int *readv;
	int *sortv;
	struct timeval begin;
	struct timeval end;
	FILE *fd;

	prgname = argv[0];

	if (argc != 3) {
		print_usage();
		exit(1);
	}

	size = atoi(argv[2]);
	if (size < 1 || size > MAX_ELEM) {
		print_error("Invalid number of elements: %d!", size);
		exit(1);
	}

	fd = fopen(argv[1], "r");
	if (!fd) {
		print_errno("fopen() failed!");
		exit(1);
	}

	readv = calloc(size, sizeof(*readv));
	sortv = calloc(size, sizeof(*sortv));
	for (i = 0; i < size; i++) {
		ret = fscanf(fd, "%d", &readv[i]);
		if (ret < 0) {
			print_errno("fscanf() failed!");
			return (ret);
		}
	}
	fclose(fd);

	gettimeofday(&begin, NULL);
	insertion_sort(readv, sortv, size);
	gettimeofday(&end, NULL);

	print_time(begin, end);

	fd = fopen("sorted_vector.txt", "w");
	if (!fd) {
		print_errno("fopen() failed!");
		exit(1);
	}

	for (i = 0; i < size; i++) {
		fprintf(fd, "%d\n", sortv[i]);
	}
	fflush(fd);
	fclose(fd);
	printf("The result can be found at file 'sorted_vector.txt'\n");

	free(readv);
	free(sortv);

	return (0);
}

