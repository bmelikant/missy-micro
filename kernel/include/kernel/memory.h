/*
 * memory.h: Kernel memory operations MISSY Microsystem Alpha 3.0
 * Provides routines to create address spaces for processes and
 * interact with the kernel heap.
 *
 *  Created on: Aug 24, 2016
 *      Author: bmelikant
 */

#ifndef MISSY_KERNEL_MEMORY_H_
#define MISSY_KERNEL_MEMORY_H_

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef _cplusplus
#include <stdbool.h>
#endif

#include <etc/multiboot.h>

#ifdef __arch_i386
#include <i386/memory/virtual_mm.h>
#endif

// define a memory space for a process
typedef struct MSPACE {

	// addresses in this memory space
	void *base_addr;
	void *heap_start;
	void *stack_top;

	// size of the binary data loaded and the heap
	size_t binary_sz;
	size_t heap_sz;
	size_t stack_sz;

	// the page directory for this process
	pdirectory *pdir;

} memory_space;

// kernel memory subsystem functions
int 	kmemory_initialize 		(struct multiboot_info *);

// kernel space functions (create / expand kernel heap)
int		kspace_initialize		();
int		kspace_brk				(void *addr);
void 	*kspace_sbrk		 	(size_t sz);

// kernel allocation routines
void *kernel_alloc 		(size_t nunits);
void *kernel_realloc 	(size_t nunits);
void kernel_free		(void *mblock);

#endif /* MISSY_KERNEL_MEMORY_H_ */
