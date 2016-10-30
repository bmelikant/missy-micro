/*
 * malloc.c: MISSY Microsystem Alpha 3.0 basic memory allocator
 * This file implements all four stdlib allocation routines
 *
 *  Created on: Aug 25, 2016
 *      Author: bmelikant
 */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

// kernel header includes if working inside the kernel
#ifdef __is_missy_kernel
#include <kernel/memory.h>
#endif

void *malloc (size_t nbytes) {

	// if we are in the kernel, call the kernel's allocator
	#ifdef __is_missy_kernel
	return kernel_alloc (nbytes);
	#endif

	return NULL;
}
