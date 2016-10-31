/*
 * stdlib.h
 *
 *  Created on: Aug 25, 2016
 *      Author: bmelikant
 */

#ifndef MISSY_LIBC_STDLIB_H_
#define MISSY_LIBC_STDLIB_H_ 1

#ifdef _cplusplus
extern "C" {
#endif

#include <stddef.h>

#ifdef __arch_i386
// typedef unsigned long	intptr_t;
#endif

// malloc function prototypes
void *malloc 	(size_t nbytes);
void *realloc 	(void *b, size_t nbytes);
void *calloc	(size_t nunits, size_t sz);
void *free		(void *b);

// string conversion prototypes
char 	*itoa_s (char *str, unsigned int number, int base);

int		atoi 	(const char *numstr);
long	strtol	(const char *num, char **endptr, int base);

#ifdef _cplusplus
}
#endif



#endif /* MISSY_LIBC_STDLIB_H_ */
