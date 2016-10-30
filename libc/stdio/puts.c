/*
 * puts.c: Print a string to stdout MISSY Microsystem Alpha 3.0
 *	- note: puts must (according to the C standard), append a newline after printing the string
 *	- the C standard only requires that a non-negative value be returned on success, so
 *	  we will choose to return the number of bytes written out
 *
 *	Created 8/14/16 Ben Melikant
 *	Last edit: 8/14/16
 */

#include <stdio.h>

// int puts (): Put a null-terminated string on the display
// inputs: str - string to print out
// returns: number of bytes printed on success, EOF on error
int puts (const char *str) {

	unsigned char *s = (unsigned char *) str;

	// loop thru the string, printing it as we go
	while (*s) {

		if (putchar (*s) == EOF)
			return EOF;
		s++;
	}

	// attempt to print out a newline
	if (putchar ('\n') == EOF)
		return EOF;

	// return the length of the print
	return (int) (s - (unsigned char *)(str)) + 1;
}