/*
 * memset.c: Fill memory block with given value (for MISSY Microsystem Alpha 3.0)
 *
 *	Created 8/14/16 Ben Melikant
 *	Last edit: 8/14/16
 */

#include <string.h>
#include <stddef.h>

// void *memset (): Fill a memory block with a given value
// inputs: dst - memory block, val - value to store, n - number of bytes to fill
// returns: * to dst
void *memset (void *dst, int val, size_t n) {

	unsigned char *d = (unsigned char *) dst;

	for (size_t i = 0; i < n; i++)
		*d = (unsigned char) val;

	return dst;
}