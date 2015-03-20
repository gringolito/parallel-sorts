// utils.h
//
// Copyright (c) 2015 Filipe Utzig. All rights reserved.
//
// Initial version by Filipe Utzig <filipeutzig@gmail.com> on 3/20/15.
//
// The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
// "SHOULD", "SHOULD NOT", "RECOMMENDED",  "MAY", and "OPTIONAL" in
// this document are to be interpreted as described in RFC 2119.
//
// Personal misc macros
//

#ifndef __FUTZIG_UTILS_H
#define __FUTZIG_UTILS_H

#include <debug.h>
#include <print.h>

#ifndef PROG_NAME
#define PROG_NAME             (prgname)
#endif
static const char *prgname;

#ifndef EVER
#define EVER                  (;;)
#endif

#ifndef MIN
#define MIN(a,b)              ((a < b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b)              ((a > b) ? (a) : (b))
#endif

#endif //__FUTZIG_UTILS_H
