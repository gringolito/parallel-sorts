// insertion_sort.h
//
// Copyright (c) 2015 Filipe Utzig. All rights reserved.
//
// Initial version by Filipe Utzig <filipeutzig@gmail.com> on 3/24/15.
//
// The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
// "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in
// this document are to be interpreted as described in RFC 2119.
//
// Insertion Sort related inline functions to be expanded in code.
//

#ifndef __FUTZIG_INSERTION_SORT_H
#define __FUTZIG_INSERTION_SORT_H

#include <stdlib.h>
#include <debug.h>

static inline void
insertion_sort (int *fuzzy, int *sort, int size)
{
	int i;
	int j;
	int tmp;

	for (i = 0; i < size; i++) {
		for (j = 0; j < i; j++) {
			if (fuzzy[i] < sort[j]) {
				tmp = sort[j];
				sort[j] = fuzzy[i];
				fuzzy[i] = tmp;
			}
		}
		sort[i] = fuzzy[i];
	}
}

#endif //__FUTZIG_INSERTION_SORT_H

