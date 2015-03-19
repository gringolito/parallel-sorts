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
#define PROG_NAME             (prgname)
#endif

static char* prgname;

void
print_usage (void)
{
	printf("Usage: %s SIZE FILE\n", PROG_NAME);
	printf("\tSIZE        Size of vector (Maximum size: %d)\n", MAX_ELEM);
	printf("\tFILE        File containing vector data\n");
}

int
main (int argc, char **argv)
{
	int size;
	int readv[MAX_ELEM + 1];
	int sortv[MAX_ELEM + 1];
	struct timeval begin;
	struct timeval end;
	FILE *fd;

	prgname = argv[0];

	if (argc != 3) {
		print_usage();
		exit (1);
	}

	size = atoi(argv[1]);
	if (size < 1 || size > MAX_ELEM) {
		print_error("Invalid number of elements: %d!", size);
		exit (1);
	}

	fd = fopen(argv[2], "r");
	if (!fd) {
		print_errno("fopen() failed!");
	}

	memset(readv, 0, size * sizeof(*readv));
	if (fread(readv, sizeof(*readv), sizeof(readv), fd) < 0) {
		print_errno("fread() failed!");
	}

	fclose(fd);

	gettimeofday(&begin, NULL);
	rank_sort(readv, sortv, size);
	gettimeofday(&end, NULL);

	print_time(begin, end);

	return (0);
}
