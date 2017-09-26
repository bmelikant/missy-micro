/*
 * process.c
 *
 *  Created on: Oct 24, 2016
 *      Author: bmelikant
 */

// std includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _cplusplus
#include <stdbool.h>
#endif

// kernel includes
#include <kernel/process.h>
#include <kernel/memory.h>

#define PROCESS_ID_START    1000

// process list entry (doubly-linked list)
typedef struct PROCESS_LIST_ENTRY {

    _process_t process_data;
    struct PROCESS_LIST_ENTRY *prev;
    struct PROCESS_LIST_ENTRY *next;

} _plist_entry_t;

// the process list!
_plist_entry_t *list_start = NULL;
unsigned long next_pid = PROCESS_ID_START;

// function declarations for list functions
_plist_entry_t *list_add_process(_process_t *proc_data);            // add a new process to the list
bool            list_remove_process(unsigned long process_id);      // remove a process from the list (deschedules and kills the process)
unsigned long   generate_process_id();                              // generate a unique id for this process

// create a new process
unsigned long create_user_process(_process_t *parent) {
    
    // create a new, blank process if parent == NULL
    if (!parent) {

        // allocate a new process, assign it a free pid, and clone in a blank page directory
        _process_t *new_process = (_process_t *) kernel_alloc (sizeof(_process_t));

        if (!new_process)
            return PROCESS_ID_INVALID;

        new_process->process_id = next_pid;
        new_process->page_directory = (pdirectory *) kernel_alloc_physical (1);

        if (!new_process->page_directory) {

            // free the process and exit
            kernel_free(new_process);
            return PROCESS_ID_INVALID;
        }

        // otherwise, use the virtual memory manager to create an address space
        if (vmmngr_create_address_space(new_process->page_directory) == -1) {

            kernel_free_physical(new_process->page_directory, 1);
            kernel_free(new_process);
        }

        return new_process->process_id;
    }
}