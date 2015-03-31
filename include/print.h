// print.h
//
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this
// notice you can do whatever you want with this stuff. If we meet some
// day, and you think this stuff is worth it, you can buy me a beer in
// return.
//
// Initial version by Filipe Utzig <filipeutzig@gmail.com> on 3/19/15.
//
// The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
// "SHOULD", "SHOULD NOT", "RECOMMENDED",  "MAY", and "OPTIONAL" in
// this document are to be interpreted as described in RFC 2119.
//
// Some useful print functions
//
//    Changelog:
//    03/31/15 - License revision
//

#ifndef __FUTZIG_PRINT_H
#define __FUTZIG_PRINT_H

#include <sys/time.h>

void print_time (struct timeval init, struct timeval end);

#endif // __FUTZIG_PRINT_H

