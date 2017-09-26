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

#define PROCESS_ID_INVALID 0x0

// process structure
typedef struct PROCESS_T {

	unsigned long 	process_id;
	pdirectory 		*page_directory;

} _process_t;

// creation and deletion of processes
unsigned long 	create_user_process(_process_t *parent);		// create a new userspace process
_process_t		*get_process_by_id(unsigned long pid);			// get a pointer to the process data for pid
void			delete_user_process(unsigned long pid);			// destroy the process given

// load code into process
unsigned long load_section(void *section_data, void *section_address, size_t section_size);

#endif /* MISSY_KERNEL_PROCESS_H_ */
