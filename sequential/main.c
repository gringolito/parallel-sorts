#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <utils.h>
#include <rank_sort.h>

#define MAX_ELEM              (100000)

void
print_usage (void)
{
	printf("Usage: %s FILE SIZE\n", PROG_NAME);
	printf("\tFILE        File containing vector data\n");
	printf("\tSIZE        Size of vector (Maximum size: %d)\n", MAX_ELEM);
}

int
main (int argc, const char **argv)
{
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
		exit (1);
	}

	size = atoi(argv[2]);
	if (size < 1 || size > MAX_ELEM) {
		print_error("Invalid number of elements: %d!", size);
		exit (1);
	}

	fd = fopen(argv[1], "r");
	if (!fd) {
		print_errno("fopen() failed!");
	}

	readv = calloc(size, sizeof(*readv));
	sortv = calloc(size, sizeof(*sortv));
	ret = fread(readv, sizeof(*readv), size, fd);
	if (ret < 0) {
		print_errno("fread() failed!");
		return (ret);
	}

	fclose(fd);

	gettimeofday(&begin, NULL);
	rank_sort(readv, sortv, size);
	gettimeofday(&end, NULL);

	print_time(begin, end);

	free(readv);
	free(sortv);

	return (0);
}
