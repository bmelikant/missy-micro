/*
 * i386_block_alloc.c: Block allocator for i386-based systems
 * For MISSY Microsystem Alpha 3.0
 *
 *	Date Created: 8/14/16 - Ben Melikant
 *	Last Edit: 8/14/16 (new file)
 *
 * Changes:
 *
 *	n/a
 */

// standard includes
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifndef _cplusplus
#include <stdbool.h>
#endif

#include "block_alloc.h"

// constant definitions
#define MEMORY_BLOCK_SIZE		4096
#define BLOCKS_PER_UNIT			32
#define BLOCK_UNIT_FULL			0xffffffff
#define LOGICAL_ADDRESS_ADJUST	0xbff00000

// macro definitions
#define memory_bitmap_clear(bit)	(m_bmp[bit/32] = (m_bmp[bit/32] &~(1 << (bit%32))))
#define memory_bitmap_set(bit)		(m_bmp[bit/32] = (m_bmp[bit/32] | (1 << (bit%32))))
#define memory_bitmap_test(bit)		(m_bmp[bit/32] & (1<<(bit%32)))

// external variables
extern uint32_t kernel_start, kernel_end;

unsigned int *m_bmp = NULL;
unsigned int mem_sz = 0, bmp_sz = 0;

static unsigned int total_blocks = 0, free_blocks = 0;

/* internal function declarations */

static unsigned int block_alloc_first_free 		();
static unsigned int block_alloc_first_free_n	(size_t n);

/* block allocation interface functions */

// unsigned int balloc_initialize (): initialize the block allocator
// inputs: mem_sz - the size of physical memory in bytes, kernel_sz, size of the kernel
// returns: 0 on error, block count on success
unsigned int balloc_initialize (unsigned int memory_sz) {

	// compute the size of the kernel
	unsigned int kernel_sz = (unsigned int)(&kernel_end) - (unsigned int)(&kernel_start);
	unsigned int kstart_phys = (unsigned int)(&kernel_start) - LOGICAL_ADDRESS_ADJUST;

	// place the memory bitmap at the end of the kernel
	mem_sz = memory_sz;
	m_bmp = ((unsigned int *)(&kernel_start)) + kernel_sz;

	printf ("Physical memory size: %d\n", memory_sz/1024);
	printf ("The kernel starts at 0x%x and ends at 0x%x\n", (unsigned int)&kernel_start, (unsigned int)&kernel_end);
	printf ("Ok, the kernel_start symbol content is 0x%x\n", (unsigned int) kernel_start);
	printf ("Kernel size reported as %d\n", kernel_sz);
	printf ("Memory address for m_bmp is 0x%x\n", (unsigned int) m_bmp);

	// set up the number of blocks in the system
	total_blocks = (unsigned int)(memory_sz / MEMORY_BLOCK_SIZE);
	free_blocks = total_blocks;

	// the bitmap size is the number of blocks in the system / 32 (sizeof int)
	bmp_sz = (total_blocks / BLOCKS_PER_UNIT) + 1;

	// mark all memory as free
	for (unsigned int i = 0; i < bmp_sz; i++)
		m_bmp[i] = 0x0;

	// compute the end of the memory bitmap address
	unsigned int kernel_total_sz = (unsigned int) kernel_sz + (bmp_sz*BLOCKS_PER_UNIT);

	// make sure the region occupied by the kernel and bitmap are not given out
	balloc_init_region (0x0, 0x100000);
	balloc_init_region (kstart_phys, kernel_total_sz);

	return total_blocks;
}

// void balloc_init_region (): Reserve a section of memory
// inputs: start - start of region, end - end of region
// returns: none
void balloc_init_region (unsigned int start, unsigned int length) {

	unsigned int start_block = (start / MEMORY_BLOCK_SIZE);
	unsigned int current_block = start_block;
	size_t block_count = (length / MEMORY_BLOCK_SIZE) + 1;

	while (current_block < (start_block + block_count)) {

		if (!memory_bitmap_test(current_block)) {

			memory_bitmap_set (current_block);
			free_blocks--;
		}

		current_block++;
	}
}

// void balloc_deinit_region (): Free a section of memory
// inputs: start - start of region, length - length of region
// returns: none
void balloc_deinit_region (unsigned int start, unsigned int length) {

	unsigned int start_block = (start / MEMORY_BLOCK_SIZE);
	unsigned int current_block = start_block;
	size_t block_count = (length / MEMORY_BLOCK_SIZE) + 1;

	while (current_block < (start_block + block_count)) {

		if (memory_bitmap_test(current_block)) {

			memory_bitmap_clear (current_block);
			free_blocks++;
		}

		current_block++;
	}
}

// void *balloc_get (): Allocate a block of memory
// inputs: none, returns: null on error, * to block on success
void *balloc_get () {

	unsigned int blk = block_alloc_first_free ();

	if (!blk)
		return NULL;

	memory_bitmap_set (blk);
	free_blocks--;

	return (void *)(blk*MEMORY_BLOCK_SIZE);
}

// void *balloc_get_n (): Allocate a series of memory blocks
// inputs: n - number of blocks, returns - same as above
void *balloc_get_n (size_t n) {

	unsigned int blk = block_alloc_first_free_n (n);

	if (!blk)
		return NULL;

	for (size_t i = 0; i < n; i++)
		memory_bitmap_set(blk+i);

	free_blocks -= n;
	return (void *)(blk*MEMORY_BLOCK_SIZE);
}

// void balloc_free (): Free a block of memory
// inputs: blk - memory block, returns: none
void balloc_free (void *blk) {

	if (!blk)
		return;

	unsigned int block = (unsigned int)(blk) / MEMORY_BLOCK_SIZE;
	memory_bitmap_clear (block);

	free_blocks++;
}

// void balloc_free_n (): Free a series of blocks of memory
// inputs: blk - memory block, n - number of blocks, returns: none
void balloc_free_n (void *blk, size_t n) {

	while (n > 0) {

		balloc_free (blk);

		blk += MEMORY_BLOCK_SIZE;
		n--;
	}
}

// accessor functions: get metrics from the allocator

unsigned int balloc_free_block_count  () { return free_blocks;  }
unsigned int balloc_used_block_count  () { return total_blocks - free_blocks;  }
unsigned int balloc_total_block_count () { return total_blocks; }

unsigned int balloc_total_memory () { return total_blocks * MEMORY_BLOCK_SIZE; }
unsigned int balloc_free_memory  () { return free_blocks * MEMORY_BLOCK_SIZE;  }

/* Internal functions */

// block_alloc_first_free (): Locate the first free block in the bitmap
// inputs: none, returns: 0 if no free block, positive value otherwise
static unsigned int block_alloc_first_free () {

	// loop thru the bitmap to find a free block
	for (size_t i = 0; i < bmp_sz; i++) {

		// if our current integer is not completely flagged, flag a new value!
		if (m_bmp[i] != BLOCK_UNIT_FULL) {

			for (size_t j = 0; j < BLOCKS_PER_UNIT; j++) {

				unsigned int bit = 1 << j;

				if (!(m_bmp[i] & bit)) {
					return (unsigned int)((i * BLOCKS_PER_UNIT) + j);
				}
			}
		}
	}

	return 0;
}

// block_alloc_first_free_n (): Locate the first free n blocks in the bitmap
// inputs: nunits - number of blocks to allocate, returns - 0 if no free blocks, nonzero otherwise
static unsigned int block_alloc_first_free_n (size_t nunits) {

	if (nunits == 0)
		return (unsigned int) NULL;
	else if (nunits == 1)
		return block_alloc_first_free ();

	for (size_t i = 0; i < bmp_sz; i++) {

		if (m_bmp[i] != BLOCK_UNIT_FULL) {

			for (size_t j = 0; j < BLOCKS_PER_UNIT; j++) {

				int bit = 1 << j;

				if (!(m_bmp[i] & bit)) {

					int startBit = (i*BLOCKS_PER_UNIT)+bit;
					size_t free = 0;

					for (size_t count = 0; count <= nunits; count++) {

						if (memory_bitmap_test(startBit+count) == 0)
							free++;

						if (free == nunits) {
							return (unsigned int)((i*BLOCKS_PER_UNIT)+j);
						}
					}
				}
			}
		}
	}

	return 0;
}
