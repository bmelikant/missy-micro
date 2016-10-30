/*
 * itoa_s.c: Transform integer into string value (MISSY Microsystem Alpha 3.0)
 *
 *	Created 8/14/16 Ben Melikant
 *	Last edit: 8/14/16
 */

#include <string.h>
#include <ctype.h>

// global variables

const char 	lettermap [] = "0123456789abcdef";
char		buffer[32];
extern int 	errno;

// char *itoa_s (char *, int, int): Transform an integer value into a string representation
// Inputs: str: destination buffer, num: number to convert, base: base of output representation
// Returns: *buffer
char *itoa_s (char *str, unsigned int num, int base) {

	int pos  = 0;
	int opos = 0;
	int top  = 0;

	// base must be < 16 and i cannot be zero
	if (num == 0 || base > 16) {

		str[0] = '0';
		str[1] = '\0';

		return str;
	}

	// now transform the number to a string
	while (num > 0) {

		buffer[pos] = lettermap[num % base];
		pos++;
		num /= base;
	}

	top = pos--;

	for (opos = 0; opos < top; pos--,opos++)
		str[opos] = buffer[pos];

	str[opos] = 0;
	return str;
}