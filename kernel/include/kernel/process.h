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

// std includes
#ifdef _cplusplus
#include <stdbool.h>
#endif

#define PROCESS_ID_INVALID 0x0

#define USERSPACE_STACK_TOP     0xbfffffff
#define USERSPACE_STACK_SZ      0x1000

// entry point typedef
typedef int (*entry_point_t)(void);

// process structure
typedef struct PROCESS_T {

	unsigned long 	process_id;
	entry_point_t	entry_point;

	pdirectory 		*page_directory;

	void			*stacktop;
	void			*stackbottom;

	bool 			is_running;

} _process_t;

// creation and deletion of processes
unsigned long 	create_user_process(_process_t *parent);		// create a new userspace process
void			delete_user_process(unsigned long pid);			// destroy the process given

_process_t		*get_process_by_id(unsigned long pid);			// get a pointer to the process data for pid

bool			run_user_process_by_id(unsigned long pid);		// run the process
bool			kill_user_process_by_id(unsigned long pid);		// kill the process

// load code / data into the process image
void *insert_section(unsigned long pid, void *section_data, void *section_address, size_t section_size);
void *initialize_stack(unsigned long pid, size_t stack_sz);

#endif /* MISSY_KERNEL_PROCESS_H_ */
