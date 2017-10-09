#ifndef _MISSY_KERNEL_BLOCK_ALLOC_H
#define _MISSY_KERNEL_BLOCK_ALLOC_H

#ifdef _cplusplus
extern "C" {
#endif

#include <stddef.h>

#define MEMORY_BLOCK_SIZE		4096

// block allocation routines

unsigned int balloc_initialize (unsigned int mem_sz);

void *balloc_get	();
void *balloc_get_n 	(size_t n);

void balloc_free	(void *blk);
void balloc_free_n 	(void *blk, size_t n);

void balloc_init_region 	(unsigned int start, unsigned int length);
void balloc_deinit_region 	(unsigned int start, unsigned int length);

unsigned int balloc_free_block_count 	();
unsigned int balloc_used_block_count 	();
unsigned int balloc_total_block_count 	();

unsigned int balloc_total_memory 	();
unsigned int balloc_free_memory		();

unsigned int balloc_kernel_size ();
unsigned int balloc_kernel_start ();

#ifdef _cplusplus
}
#endif

#endif
