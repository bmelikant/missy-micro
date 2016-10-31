/*
 * strcpy.c: String copy library file
 *
 *  Created on: Oct 30, 2016
 *      Author: bmmel
 */

#include <string.h>

// char *strncpy (char *dst, const char *src, size_t bytes): Copy a string of bytes size
// inputs: you know what they do...
// returns: you know this too, it's standard!!
char *strncpy (char *dst, const char *src, size_t bytes) {

	char *dest = dst;

	for ( ; bytes > 0 && *src != '\0'; bytes--)
		*dst++ = *src++;

	while (bytes > 0) {

		*dst++ = '\0';
		bytes--;
	}

	return dest;
}

// char *strcpy (char *dst, const char *src): Copy a source string to a destination string
char *strcpy (char *dst, const char *src) { return strncpy (dst, src, strlen (src)); }
