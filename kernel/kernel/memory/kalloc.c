/*
 * kalloc.c
 *
 *  Created on: Oct 23, 2016
 *      Author: bmelikant
 */

// standard header includes
#include <stddef.h>
#include <stdio.h>

// kernel includes
#include "../include/kmemory.h"

// constants
#define MIN_REQUEST_SIZE 1024

// memory header definition
typedef union memory_header {

	struct {

		union memory_header *next;
		size_t sz;

	} s;

	// force alignment
	unsigned long align;

} m_header;

// internal functions
static m_header *morecore (size_t sz);

// internal variables
static m_header base;
static m_header *free_hdr = NULL;

// void *kernel_alloc (): Allocate nbytes bytes from the kernel heap
// inputs: nbytes - number of bytes to allocate
// returns: null on error, ptr to memory otherwise
void *kernel_alloc (size_t nbytes) {

	m_header *hdr, *prev_hdr;
	size_t alloc_sz = nbytes + sizeof(m_header);

	// if no list exists, start a new list. This will force an sbrk
	// call immediately upon loop entry
	if ((prev_hdr = free_hdr) == NULL) {

		base.s.next = free_hdr = prev_hdr = &base;
		base.s.sz = 0;
	}

	// look for a free header of sufficient size to hold the allocation
	for (hdr = prev_hdr->s.next; ; prev_hdr = hdr, hdr = hdr->s.next) {

		// this header is big enough
		if (hdr->s.sz >= alloc_sz) {

			// exact size
			if (hdr->s.sz == alloc_sz)
				prev_hdr->s.next = hdr->s.next;

			else {

				hdr->s.sz -= alloc_sz;
				hdr = (m_header *) ((char*)(hdr)+hdr->s.sz);
				hdr->s.sz = alloc_sz;
			}

			free_hdr = prev_hdr;
			return (void *)(hdr+1);
		}

		// if the list wrapped around, grab more memory
		if (hdr == free_hdr) {

			if ((hdr = morecore(alloc_sz)) == NULL)
				return NULL;
		}
	}

	// we should NEVER reach this point; failsafe
	return NULL;
}

// void kernel_free (): Free a memory block from the allocator
// inputs: mblock - ptr to block to free
// returns: none
void kernel_free (void *mblock) {

	m_header *blk, *hdr;
	blk = (m_header *) mblock - 1;

	for (hdr = free_hdr; !(blk > hdr && blk < hdr->s.next); hdr = hdr->s.next)
		if (hdr >= hdr->s.next && (blk > hdr || blk < hdr->s.next))
			break;

	if (blk + blk->s.sz == hdr->s.next) {

		blk->s.sz += hdr->s.next->s.sz;
		blk->s.next = hdr->s.next->s.next;
	}
	else
		blk->s.next = hdr->s.next;

	if (hdr + hdr->s.sz == blk) {

		hdr->s.sz += blk->s.sz;
		hdr->s.next = blk->s.next;
	}
	else
		hdr->s.next = blk;

	free_hdr = hdr;
}

// m_header *morecore (): Get more memory on the kernel heap
// inputs: nu - number of requested bytes
// returns: NULL on error, addr of new header on success
static m_header *morecore (size_t nu) {

	// adjust for the header size
	nu += sizeof(m_header);

	if (nu < MIN_REQUEST_SIZE)
		nu = MIN_REQUEST_SIZE;

	m_header *next = (m_header *)(kspace_sbrk(nu));

	if (next == (m_header *) -1)
		return NULL;

	next->s.sz = nu;
	kernel_free((void *)next+sizeof(m_header));
	return next;
}
