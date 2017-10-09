/*
 * virtual_mm.h
 *
 *  Created on: Aug 17, 2016
 *      Author: bmelikant
 */

#ifndef MISSY_KERNEL_VIRTUAL_MM_H_
#define MISSY_KERNEL_VIRTUAL_MM_H_

// standard includes

#ifndef _cplusplus
#include <stdbool.h>
#endif

#include <string.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __arch_i386

typedef uint32_t pdirectory;
typedef uint32_t pdir_entry;
typedef uint32_t ptable_entry;

typedef uint32_t phys_addr;
typedef uint32_t virt_addr;

#define PAGE_DIRECTORY_SIZE 0x1000

#endif


/* platform-independent vmmngr routines */

int vmmngr_initialize (uint32_t kernel_start, size_t kernel_total_sz);

// create and delete address spaces, and switch to different address spaces
int vmmngr_create_address_space 	(pdirectory *pdir);
int vmmngr_destroy_address_space 	(pdirectory *pdir);
int vmmngr_switch_address_space		(pdirectory *pdir);

// map pages into and out of the current address space
int 		vmmngr_map_page 	(phys_addr p, virt_addr v);
phys_addr	vmmngr_unmap_page	(virt_addr v);

#endif /* MISSY_KERNEL_VIRTUAL_MM_H_ */
