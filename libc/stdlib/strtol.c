/*
 * strtol.c: Transform a string into an integer value (MISSY Microsystem Alpha 3.0)
 *
 *	Created 8/14/16 Ben Melikant
 *	Last edit: 8/14/16
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

#ifndef _cplusplus
#include <stdbool.h>
#endif

// errno
extern int errno;

// long int strtol (): Convert a string to a long integer value
long int strtol (const char *num, char **endptr, int base) {

	const char *numstr = num;
	register long int retval = 0;
	register long int cutoff = 0, cutlimit = 0;

	bool negative = false;

	// verify base
	if ((base != 0) && ((base < 2) || (base > 36))) {

		errno = ERANGE;
		return 0;
	}

	// read all the whitespace off the string
	if (isspace (*numstr))
		while (isspace (*numstr++))
			;

	// look for optional + or -
	if (*numstr == '-') {

		negative = true;
		numstr++;

	} else if (*numstr == '+')
		numstr++;

	// if base == 16 check for 0x prefix
	if (base == 16) {

		// *num should be 0, *num+1 should be x
		if (*numstr == '0' && *numstr+1 == 'x')
			numstr += 2;
	}

	// if base == 0 check for optional prefixes
	else if (base == 0) {

		// check as above for base 16
		if (*numstr == '0' && *numstr+1 == 'x') {

			base = 16;
			numstr += 2;
		}

		// also check for optional O prefix (octal)
		if (*numstr == 'O') {

			base = 8;
			numstr++;
		}
	}

	// this field indicates whether a digit was consumed
	cutoff = negative ? (unsigned long) LONG_MIN : LONG_MAX;
	cutlimit = cutoff % (unsigned long) base;
	cutoff /= (unsigned long) base;
	register int any = 0;

	// start parsing out the numbers
	while (*numstr) {

		register int c = *numstr;

		// translate digit to integer
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10 ;
		else
			break;

		// if we hit a character that can't be used, break
		if (c >= base)
			break;

		// make sure we aren't out of range
		if (any < 0 || retval > cutoff || (retval == cutoff && c > cutlimit))
			any = -1;

		else {

			any = 1;
			retval *= base;
			retval += c;
		}

		numstr++;
	}

	// if the last character was NOT consumed, return max and set error
	if (any < 0) {

		retval = negative ? LONG_MIN : LONG_MAX;
		errno = ERANGE;
	}

	// if an endptr was requested, set it
	if (endptr)
		*endptr = (char *)(any ? numstr - 1 : num);

	// return a negative number if negative
	if (negative)
		return -(retval);

	// otherwise just return the value
	return retval;
}
