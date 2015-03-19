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

int
main (int argc, char **argv)
{
	int size;
	int readv[MAX_ELEM];
	int sortv[MAX_ELEM];
	struct timeval tvi;
	struct timeval tvf;
	FILE *fd;

	if (argc != 3) {
		print_usage();
		exit (1);
	}

	size = atoi(argv[1]);
	if (size < 1 || size >= MAX_ELEM) {
		print_error("Invalid number of elements: %d!", size);
		exit (1);
	}

	fd = fopen(argv[0], "r");
	if (!fd) {
		print_errno("fopen() failed!");
	}

	memset(readv, 0, sizeof(readv)*sizeof(*readv));
	if (fread(readv, sizeof(*readv), sizeof(readv), fd) < 0) {
		print_errno("fread() failed!");
	}

	fclose(fd);

	gettimeofday(&tvi, NULL);
	rank_sort(readv, sortv, size);
	gettimeofday(&tvf, NULL);

	print_time(&tvi, &tvf);

	return (0);
}
