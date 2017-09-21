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

// kernel private includes
#include <include/kmemory.h>

// process structure
struct process {

	unsigned long process_id;

};

#endif /* MISSY_KERNEL_PROCESS_H_ */
