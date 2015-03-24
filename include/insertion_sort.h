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
	int x;

	for (i = 0; i < vetorord; i++) {
		valor = temp[i];
		for (j = 0; j < i; j++) {
			if (valor < vector[j]) {
				aux = vector[j];
				vector[j] = valor;
				valor = aux;
			}
		}
		vector[i] = valor;
	}
}

#endif //__FUTZIG_INSERTION_SORT_H

