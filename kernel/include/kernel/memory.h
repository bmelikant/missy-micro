/*
 * memory.h
 *
 *  Created on: Aug 26, 2017
 *      Author: bmelikant
 */

#ifndef KERNEL_INCLUDE_KERNEL_MEMORY_H_
#define KERNEL_INCLUDE_KERNEL_MEMORY_H_

#ifdef __arch_i386
#include <i386/include/memory_virt.h>
#endif

#include <include/kmemory.h>

// memory space structure
typedef struct MEMORY_SPACE {

	unsigned long pid;		// process to which the memory belongs
	pdirectory *page_dir;	// the page directory for this memory

	void *entry;			// entry point of the program or exe
	void *heap_start;		// where the heap starts
	void *stack_mem;		// where the stack starts

} mspace;

#endif /* KERNEL_INCLUDE_KERNEL_MEMORY_H_ */
