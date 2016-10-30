/*
 * strlen.c: Implement strlen standard library function (MISSY Microsystem Alpha 3.0)
 *
 *	Created 8/14/16 Ben Melikant
 *	Last edit: 8/14/16
 */

#include <string.h>
#include <stddef.h>

// size_t strlen (): get the length of a string
size_t strlen (const char *s) {

	char *s2 = (char *) s;

	while (*s2++)
		;

	return (size_t)(s2 - s);
}