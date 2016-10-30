/*
 * memcpy.c: Copy memory blocks (for MISSY Microsystem Alpha 3.0)
 *
 *	Created 8/14/16 Ben Melikant
 *	Last edit: 8/14/16
 */

#include <string.h>
#include <stddef.h>

// void *memcpy (): Copy a memory block to another memory block
// inputs: dst - destination block, src - source block, n - blocks to copy
// returns: * to dest block
void *memcpy (void *dst, const void *src, size_t n) {

	unsigned char *d = (unsigned char *) dst;
	unsigned char *s = (unsigned char *) src;

	// this loop copies the memory over while i < n
	for (size_t i = 0; i < n; (*d++ = *s++), i++)
		;

	return dst;
}