// main.c
//
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this
// notice you can do whatever you want with this stuff. If we meet some
// day, and you think this stuff is worth it, you can buy me a beer in
// return.
//
// Initial version by Filipe Utzig <filipeutzig@gmail.com> on 4/9/15.
//
// The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
// "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in
// this document are to be interpreted as described in RFC 2119.
//
// Yet another sort app using Merge Sort algorithm
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
#include <merge_sort.h>

#define MAX_ELEM                       (500000)
#define FILENAME                       "sorted_vector.txt"

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
	int *readv;
	size_t size;
	struct timeval begin;
	struct timeval end;
	FILE *fd;

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

	fd = fopen(argv[1], "r");
	if (!fd) {
		print_errno("fopen() failed!");
		exit(1);
	}

	readv = calloc(size, sizeof(*readv));
	for (i = 0; (size_t) i < size; i++) {
		ret = fscanf(fd, "%d", &readv[i]);
		if (ret < 0) {
			print_errno("fscanf() failed!");
			return (ret);
		}
	}
	fclose(fd);

	gettimeofday(&begin, NULL);
	merge_sort(readv, 0, size);
	gettimeofday(&end, NULL);

	print_time(begin, end);

	SAVE_RESULTS(RESULTS_WRITE, readv, size);

	printf("The result can be found at file '%s'\n", FILENAME);

	free(readv);

	return (0);
}

