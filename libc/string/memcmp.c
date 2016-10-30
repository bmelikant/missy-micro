/*
 * memcmp.c: Compare two blocks of memory (for MISSY Microsystem Alpha 3.0)
 *
 *	Created 8/14/16 Ben Melikant
 *	Last edit 8/14/16
 */

#include <string.h>
#include <stddef.h>

// int memcmp (): Compare two blocks of memory
// inputs: s1 - first block, s2 - second block, n - number of bytes to compare
// returns: <0 if *s1 < *s2, >0 if *s1 > *s2, 0 if blocks equal
int memcmp (const void *s1, const void *s2, size_t n) {

	unsigned char *src1 = (unsigned char *) s1;
	unsigned char *src2 = (unsigned char *) s2;

	// look for non-matching values
	for (size_t i = 0; i < n; i++)
		if (*src1++ != *src2++)
			return *src1 - *src2;	// return the difference between non-matching vals

	return 0;
}