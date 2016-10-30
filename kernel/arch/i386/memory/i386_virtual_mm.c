/*
 * i386_virtual_mm.c: Manage process address spaces on i386 platforms
 * MISSY Microsystem Alpha 3.0
 *
 *  Created on: Aug 18, 2016
 *      Author: bmelikant
 */

// kernel includes
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "block_alloc.h"
#include "virtual_mm.h"

#ifndef _cplusplus
#include <stdbool.h>
#endif

// constant definitions

#define I386_PTE_PRESENT	0x0001
#define I386_PTE_RW			0x0002
#define I386_PTE_USER		0x0004
#define I386_PTE_WRITETHRU	0x0008
#define I386_PTE_NOCACHE	0x0010
#define I386_PTE_ACCESSED	0x0020
#define I386_PTE_DIRTY		0x0040
#define I386_PTE_GLOBAL		0x0100

#define I386_KERNEL_SECTION_CODE	I386_PTE_PRESENT
#define I386_KERNEL_SECTION_DATA	I386_PTE_PRESENT | I386_PTE_RW
#define I386_KERNEL_SECTION_RODATA	I386_KERNEL_SECTION_CODE

#define I386_USER_SECTION_CODE		I386_PTE_PRESENT | I386_PTE_USER
#define I386_USER_SECTION_DATA		I386_PTE_PRESENT | I386_PTE_USER | I386_PTE_RW
#define I386_USER_SECTION_RODATA	I386_USER_SECTION_CODE

#define I386_ENTRIES_PER_PTABLE 	1024
#define I386_ENTRIES_PER_PDIR 		I386_ENTRIES_PER_PTABLE

#define I386_TEMP_PDIR_VIRTUAL		(0xD0000000 - 0x1000)
#define I386_PDIR_KERNEL_START		768
#define I386_RECURSIVE_ENTRY		1023

#define I386_BLOCK_SIZE		0x1000
#define I386_PTABLE_SIZE	(I386_BLOCK_SIZE*1024)
#define I386_PDIR_SIZE		(I386_PTABLE_SIZE*1024)

// structures

struct pg_inf {

	uint32_t ptable;
	uint32_t page;
};

// internal data

static pdirectory *pdir_virtual 	= NULL;
static pdirectory *pdir_physical 	= NULL;

// internal function declarations
static struct pg_inf 	i386_virtual_to_pgidx 	(uint32_t addr);

// asm functions for setting / getting page directory base register
static inline void i386_set_pdbr (pdirectory *pdir_addr) { __asm__ volatile ("mov %0,%%cr3\n" :: "b"(pdir_addr)); }
static inline pdirectory *i386_get_pdbr (void) { pdirectory *pdir = NULL; __asm__ volatile ("mov %%cr3,%0\n" : "=r"(pdir)); return pdir; }

/* interface functions */

// int vmmngr_initialize (): Initialize the address space manager
// inputs: none
// returns: 0 on success, nonzero on failure (sets errno)
int vmmngr_initialize () {

	// we must set up a new page directory when we enable paging
	pdirectory *new_pdir_phys = (pdirectory *) balloc_get ();
	pdirectory *new_pdir_virt = (pdirectory *) I386_TEMP_PDIR_VIRTUAL;

	// get the physical address of the orignial page directory (to copy it!)
	pdir_virtual 	= (pdirectory *) 0xfffff000;
	pdir_physical 	= (pdirectory *) (pdir_virtual[I386_RECURSIVE_ENTRY] &~ 0xfff);

	// we need to map the new page directory to a temporary address for writing
	if (vmmngr_map_page ((phys_addr) new_pdir_phys, (virt_addr) new_pdir_virt) == -1)
		return -1;

	// let's copy over the old page directory into the new one
	memcpy (new_pdir_virt, pdir_virtual, 1024*sizeof(pdir_entry));

	// map the page directory's last entry to itself: fractal mapping of page tables!!
	// this way we can access any page table by accessing it's virtual address
	new_pdir_phys[1023] = ((pdir_entry) (new_pdir_phys) | 3);

	// transfer the new page directory into the pdbr
	i386_set_pdbr (new_pdir_phys);
	pdir_physical = new_pdir_phys;

	// now point the physical page directory to it's virtual address and unmap the temporary page
	pdir_virtual = (pdirectory *) 0xfffff000;
	vmmngr_unmap_page ((virt_addr) new_pdir_virt);

	return 0;
}

// create a new virtual address space; store it in the given page directory
// inputs - p: must point to a 4Kb allocated region of memory!
// returns: 0 on success, -1 on failure
int vmmngr_create_address_space (pdirectory *p) {

	// we should already have a page directory allocated at *p
	if (!p)
		return -1;

	// copy the kernel into the high address space
	pdirectory *new_pdir_phys = p;
	pdirectory *new_pdir_virt = (pdirectory *) I386_TEMP_PDIR_VIRTUAL;

	if (vmmngr_map_page ((phys_addr) new_pdir_phys, (virt_addr) new_pdir_virt) == -1)
		return -1;

	// the first Mb should be identity mapped and protected for
	// ring 0 processes ONLY!
	for (int idx = 0; idx < 256; idx++)
		new_pdir_virt[idx] = pdir_virtual[idx];

	// now map in the kernel, same deal. Supervisor process ONLY! (ring 0)
	for (int idx = I386_PDIR_KERNEL_START; idx < 1024; idx++)
		new_pdir_virt[idx] = pdir_virtual[idx];

	// that's it, the address space is all set up!
	return 0;
}

// switch address spaces. changes to requested page directory
// inputs - p: must point to a valid page directory (otherwise causes crash!)
// returns: 0 on success, -1 on failure
int vmmngr_switch_address_space (pdirectory *p) {

	if (!p)
		return -1;

	// just set the new pdbr!
	i386_set_pdbr (p);
	return 0;
}

// i386_vmm_map_page (): Map a physical address into memory
int vmmngr_map_page (phys_addr phys, virt_addr virt) {

	struct pg_inf page_data = i386_virtual_to_pgidx (virt);

	// If the page is allocated, map it
	if (pdir_virtual[page_data.ptable] & 1) {

		// get the virtual address of the page table
		virt_addr *p_table = (virt_addr *) (0xFFC00000 + (page_data.ptable * 0x1000));

		// the page is not mapped; map it
		if (!(p_table[page_data.page] & 1))
			p_table[page_data.page] = phys | 3;

		// the page is already mapped
		else
			return -1;
	}

	// otherwise, make a new page table, then map
	else {

		ptable_entry *ptable_phys = (ptable_entry *) balloc_get ();
		ptable_entry *ptable_virt = (ptable_entry *) (0xFFC00000 + (page_data.ptable * 0x1000));
		pdir_virtual[page_data.ptable] = ((phys_addr) (ptable_phys) | 3);

		memset (ptable_virt, 0x0, I386_ENTRIES_PER_PTABLE*4);
		ptable_virt[page_data.page] = ((phys_addr) (phys) | 3);
	}

	// reload cr3 to force a tlb flush
	i386_set_pdbr (pdir_physical);
	return 0;

}

// i386_vmm_unmap_page () Unmap a virtual address
phys_addr vmmngr_unmap_page (virt_addr virt) {

	struct pg_inf page_data = i386_virtual_to_pgidx (virt);
	phys_addr phys = 0;

	// the directory entry is mapped
	if (pdir_virtual[page_data.ptable] & 1) {

		// find the page table
		ptable_entry *p_table = (ptable_entry *) (0xFFC00000 + (page_data.ptable * 0x1000));

		// the page is mapped; unmap it
		if (p_table[page_data.page] & 1) {

			phys = (phys_addr) (p_table[page_data.page] &~ 0xfff);	// mask off the lower 12 bits
			p_table[page_data.page] = 0x00;
		}

		int i = 0;

		// check for more PTEs in the table
		for ( ; i < 1024; i++)
			if (p_table[i] & 1)
				break;

		// if there were none, deallocate the page table
		if (i >= 1024) {

			balloc_free ((void *) (pdir_virtual[page_data.ptable] & 0xFFFFF000));
			pdir_virtual[page_data.ptable] = 0x00;
		}
	}

	// force a tlb flush
	i386_set_pdbr (pdir_physical);

	// return the physical address
	return phys;
}

/* Internal functions */

// i386_virtual_to_pgidx (): Translate a virtual address into a ptable and page
static struct pg_inf i386_virtual_to_pgidx (virt_addr addr) {

	struct pg_inf pgdat;

	addr &= ~0xfff;
	pgdat.ptable = addr / I386_PTABLE_SIZE;
	pgdat.page = (addr % I386_PTABLE_SIZE) / I386_BLOCK_SIZE;

	return pgdat;
}
