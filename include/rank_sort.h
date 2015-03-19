// rank_sort.h
//
// Copyright (c) 2015 Filipe Utzig. All rights reserved.
//
// Initial version by Filipe Utzig <filipeutzig@gmail.com> on 3/19/15.
//
// The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
// "SHOULD", "SHOULD NOT", "RECOMMENDED",  "MAY", and "OPTIONAL" in
// this document are to be interpreted as described in RFC 2119.
//
// Rank sort related inline functions to be expanded in code.
//

#ifndef __FUTZIG_RANK_SORT_H
#define __FUTZIG_RANK_SORT_H

#include <stdlib.h>
#include <debug.h>

inline void
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

inline int
merge_vector (int *vector, int begin, int mid, int end)
{
	int ib = begin;
	int im = mid;
	int j;
	int size = end - begin;
	int *tmp;

	tmp = calloc(size, sizeof(*tmp));
	if (!tmp) {
		print_errno("calloc() failed!");
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

	return (0);
}

#endif //__FUTZIG_RANK_SORT_H
