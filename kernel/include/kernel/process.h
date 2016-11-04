/*
 * process.h: Process interface for MISSY Microsystem Alpha 3.0
 *
 *  Created on: Aug 18, 2016
 *      Author: Ben Melikant
 */

#ifndef MISSY_KERNEL_PROCESS_H_
#define MISSY_KERNEL_PROCESS_H_

// kernel includes
#include <include/cpu.h>

#include <kernel/memory.h>

// process structure
struct process {

	int process_id;

	memory_space 	mspace;
};

#endif /* MISSY_KERNEL_PROCESS_H_ */
