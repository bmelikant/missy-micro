/*
 * strcmp: String comparison library functions
 *
 *  Created on: Oct 30, 2016
 *      Author: bmmelikant
 */

#include <string.h>

// int strncmp (): Return the difference of two strings
int strncmp (const char *s1, const char *s2, size_t len) {

	for ( ; *s1 == *s2 && len > 0; s1++, s2++, len--)
		if (*s1 == '\0')
			return 0;

	if (len > 0)
		return *s1 - *s2;
	else
		return 0;
}

// int strcmp (): Return the difference of two strings
int strcmp (const char *s1, const char *s2) {

	for ( ; *s1 == *s2; s1++, s2++)
		if (*s1 == '\0')
			return 0;

	return *s1 - *s2;
}
