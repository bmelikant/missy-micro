/*
 * kmemory.c
 *
 *  Created on: Aug 22, 2017
 *      Author: bmelikant
 */

// kernel private includes
#include <errno.h>
#include <include/exception.h>
#include <include/multiboot.h>
#include <include/balloc.h>
#include <include/kmemory.h>

// kernel public includes
#include <kernel/tty.h>

// i386 architecture includes
#ifdef __build_i386
#include <i386/include/memory_virt.h>
#endif

// kernel heap definitions
#define KHEAP_PAGE_START	0xd0000000
#define KHEAP_PAGE_MAX		0xeffff000
#define KERNEL_PAGE_SIZE	4096

// get the end-of-kernel address and set the current kernel break address
extern uint32_t kernel_end;
uint32_t kernel_break 		= (uint32_t) &kernel_end;
uint32_t kheap_last_page	= KHEAP_PAGE_START - KERNEL_PAGE_SIZE;

// internal variables
void *kheap_base = NULL;

// int kmemory_initialize (): Initialize the kernel memory manager
// and set up the kernel's address space
// inputs: none
// returns: none (may halt system on error)
void kmemory_initialize () {

	terminal_puts("Kernel memory manager is starting up...");

	// get the memory size from multiboot and turn it into bytes from Kb
	unsigned int mem_sz = multiboot_get_memsz()*1024;

	// try to initialize the block allocator
	if (!mem_sz || !balloc_initialize(mem_sz))
		kernel_panic ("Block allocator failed to initialize!");

	// process the memory map
	mmap_data map_entry;

	if (!multiboot_get_mmap_next (&map_entry))
		kernel_panic ("No memory map was provided by the bootloader!");

	// loop thru the map
	do {

		if (map_entry.type != 1)
			balloc_init_region ((unsigned int) map_entry.base, (unsigned int) map_entry.length);

	} while (multiboot_get_mmap_next (&map_entry));

	if (vmmngr_initialize () == -1)
		kernel_panic ("Virtual memory subsystem failed to initialize!");

	terminal_puts ("Done!");
}
// int kspace_initialize (): Initialize the kernel's address space
// inputs: none
// returns: 0 on success, -1 on error (sets errno)
void kspace_initialize () {

	// create the start of the kernel heap. Initial heap is 4kb
	kheap_base = balloc_get ();

	if (!kheap_base)
		kernel_panic ("Failed to allocate space for kernel heap");

	// map the page into the kernel's virtual address space
	vmmngr_map_page ((phys_addr) kheap_base, (virt_addr) KHEAP_PAGE_START);

	// expand the initial heap into the virtual address space
	kernel_break = KHEAP_PAGE_START;
	kheap_last_page = KHEAP_PAGE_START;
}

// int kspace_brk (): Expand the kernel heap by setting the break (data segment end)
// inputs: addr - new address of kernel break
// returns: 0 on success, -1 on error (sets errno to ENOMEM)
int kspace_brk (void *addr) {

	uint32_t bp = (uint32_t) addr;

	// if the new address is outside the available heap space, exit
	if (bp > KHEAP_PAGE_MAX) {

		errno = ENOMEM;
		return -1;
	}

	// if the new address is within the last allocated page,
	// just set the break to the new address
	if ((bp &~ 0xfff) == kheap_last_page)
		kernel_break = bp;

	// we are outside our page boundary so
	// time to allocate some new pages
	else if ((bp &~ 0xfff) > kheap_last_page){

		size_t nblocks = (((bp &~ 0xfff) - kheap_last_page) >> 12);

		for (size_t i = 0; i < nblocks; i++) {

			void *kheap_block = balloc_get ();

			if (!kheap_block) {

				errno = ENOMEM;
				return -1;
			}

			kheap_last_page += KERNEL_PAGE_SIZE;

			if (vmmngr_map_page ((phys_addr) kheap_block, (virt_addr) kheap_last_page) == -1) {

				balloc_free (kheap_block);
				errno = ENOMEM;
				return -1;
			}
		}

		// change the break address
		kernel_break = bp;
	}

	return 0;
}

// void *kspace_sbrk (size_t increment): expand the kernel heap by increment bytes
// inputs: increment - number of bytes to expand
// returns: location of old break address
void *kspace_sbrk (size_t increment) {

	void *old_brk = (void *)(kernel_break);

	// use the old kernel break to set the new break
	if (kspace_brk ((void *)(kernel_break+increment)) != 0) {

		errno = ENOMEM;
		return (void *) -1;
	}

	return old_brk;
}

// void *kernel_request_mapping(): Request a mapping to a virtual page for a physical address
// inputs: addr_phys - the physical address to map, size_t blocks - number of 4K blocks to map
// returns: NULL on error, virtual address on success
void *kernel_request_temp_mapping (void *addr_phys, size_t blocks) {

	// check for a free mapping beyond the kernel heap of sufficient size
	uint32_t free_mapping = (uint32_t) kheap_last_page+KERNEL_PAGE_SIZE;
	void *return_val = (void *) free_mapping;

	// try to map the space in
	for (size_t i = 0; i < blocks; i++) {

		// roll back changes to the page table on failure
		if (vmmngr_map_page(addr_phys, free_mapping) == -1 && i > 0) {

			for (size_t j = i-1; j > 0; j--) {

				vmmngr_unmap_page(free_mapping);
				free_mapping -= KERNEL_PAGE_SIZE;
			}

			errno = ENOMEM;
			return NULL;
		}

		// otherwise increment the page and do the next one!
		free_mapping += KERNEL_PAGE_SIZE;
	}

	return return_val;
}

// void kernel_release_temp_mapping(): Release a temporary mapping
// inputs: void *addr_virt - virtual address to release
// returns: none
void kernel_release_temp_mapping(void *addr_virt, size_t blocks) {

	uint32_t unmap = (uint32_t) addr_virt;

	for (size_t i = 0; i < blocks; i++) {

		vmmngr_unmap_page(unmap);
		unmap += KERNEL_PAGE_SIZE;
	}
}