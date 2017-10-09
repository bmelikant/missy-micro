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
#include <errno.h>

#ifndef _cplusplus
#include <stdbool.h>
#endif

// kernel includes
#include <kernel/process.h>
#include <kernel/memory.h>

#define PROCESS_ID_START    1000

// process list entry (doubly-linked list)
typedef struct PROCESS_LIST_ENTRY {

    _process_t *process_data;

    struct PROCESS_LIST_ENTRY *prev;
    struct PROCESS_LIST_ENTRY *next;

} _plist_entry_t;

// the process list!
_plist_entry_t *list_start = NULL;
unsigned long next_pid = PROCESS_ID_START;

// assembly routines to kickoff and kill processes
unsigned long kickoff_process(entry_point_t entrypt, void *stacktop, void *stackbottom);

// function declarations for list functions
_plist_entry_t *list_insert_process(_process_t *proc_data);             // add a new process to the list
_process_t     *list_get_process(unsigned long pid);                    // find a process in the list
bool            list_remove_process(unsigned long pid);                 // remove a process from the list (deschedules and kills the process)
unsigned long   generate_process_id();                                  // generate a unique id for this process

// create a new process
unsigned long create_user_process(_process_t *parent) {
    
    // create a new, blank process if parent == NULL
    if (!parent) {

        // allocate a new process, assign it a free pid, and clone in a blank page directory
        _process_t *new_process = (_process_t *) kernel_alloc (sizeof(_process_t));

        if (!new_process) {
            
            errno = ENOMEM;
            return PROCESS_ID_INVALID;
        }

        new_process->process_id = next_pid++;
        new_process->page_directory = (pdirectory *) kernel_alloc_physical (1);

        if (!new_process->page_directory) {

            // free the process and exit
            kernel_free(new_process);
            errno = ENOMEM;
            return PROCESS_ID_INVALID;
        }

        // otherwise, use the virtual memory manager to create an address space
        if (vmmngr_create_address_space(new_process->page_directory) == -1 || 
            !list_insert_process(new_process)) {

            kernel_free_physical(new_process->page_directory, 1);
            kernel_free(new_process);

            errno = ENOSYS;
            return PROCESS_ID_INVALID;
        }

        return new_process->process_id;
    }

    else {

        return PROCESS_ID_INVALID;
    }
}

// void delete_user_process(): Delete a process running in userspace
// inputs: pid - process id
// returns: none
void delete_user_process(unsigned long pid) {

    _process_t *user_process = list_get_process(pid);
    
    // if the process exists...
    if (user_process) {

        // and it isn't running...
        if (!user_process->is_running) {

            // delete it from the list and clean up it's memory
            list_remove_process(pid);
            kernel_free_physical(user_process->page_directory, 1);
            kernel_free(user_process);
        }
    }
}


// run the process with the given ID
bool run_user_process_by_id(unsigned long pid) {

    if (list_start) {
    
        for (_plist_entry_t *current = list_start; current != NULL; current = current->next) {

            if (current->process_data->process_id == pid) {
                /*
                kickoff_process(current->process_data->entry_point,
                    current->process_data->stacktop,
                    current->process_data->stackbottom);
                */
                return true;
            }
        }
    }

    return false;
}

// get a process by its id
_process_t *get_process_by_id(unsigned long pid) {

    if (list_start) {

        for (_plist_entry_t *current = list_start; current != NULL; current = current->next)
            if (current->process_data->process_id == pid)
                return current->process_data;
    }

    return NULL;
}

// create the stack for the process
void *initialize_stack(unsigned long pid, size_t stack_sz) {

    _process_t *p = get_process_by_id(pid);

    if (!p)
        return NULL;

    
    // reserve some physical memory for the stack and map it in
    size_t stack_blocks = ((stack_sz / MEMORY_BLOCK_SIZE) + 1);
    void *stack_phys = balloc_get_n(stack_blocks);

    terminal_printf("The block was allocated at 0x%x\n", (unsigned int) stack_phys);
    
    if (!stack_phys)
        return NULL;

    // grab the top of the stack in userspace
    unsigned int stack_top = USERSPACE_STACK_TOP;

    // map it into memory @ the stack top
    for (size_t i = 0; i < stack_blocks; i++) {
    
        unsigned int stack_virt = (USERSPACE_STACK_TOP - (i * MEMORY_BLOCK_SIZE)) &~ 0xfff;

        if (vmmngr_map_page(stack_phys, stack_virt) == -1) {

            // roll back the changes
            for (size_t j = i; j > 0; j--) {

                unsigned int reverse_virt = (USERSPACE_STACK_TOP - (j * MEMORY_BLOCK_SIZE)) &~ 0xfff;
                vmmngr_unmap_page(reverse_virt);
            }

            balloc_free_n(stack_phys, stack_blocks);
            return NULL;
        }
    }

    return (void *) stack_top;
}

/* internal functions */

// _plist_entry_t *list_insert_process(): Insert a new process into the list
// inputs: process_data - a pointer to a _process_t structure that holds the process data
// returns: pointer to the process in the list, NULL if process couldn't be inserted. sets errno on error
_plist_entry_t *list_insert_process(_process_t *proc_data) {

    // if the first node isn't allocated, set it up
    if (!list_start) {

        // allocate a new node, set its pointers to null, and copy the data over
        list_start = (_plist_entry_t *) kernel_alloc(sizeof(_plist_entry_t));

        if (!list_start) {
            
            errno = ENOMEM;
            return NULL;
        }

        list_start->prev = NULL;
        list_start->next = NULL;

        list_start->process_data = proc_data;
        list_start->process_data->is_running = false;

        return list_start;
    }

    else {

        _plist_entry_t *current = list_start;

        for ( ; current->next != NULL; current = current->next)
            ;

        current->next = (_plist_entry_t *) kernel_alloc(sizeof(_plist_entry_t));
        
        if (!current->next) {
            
            errno = ENOMEM;
            return NULL;
        }

        current->next->prev = current;
        current = current->next;

        current->next = NULL;
        current->process_data = proc_data;
        current->process_data->is_running = false;

        return current;
    }
}

// _process_t *list_get_process(unsigned long pid): Look up a process in the list
// inputs: pid - the process's id
// returns: pointer to the process if found, NULL otherwise
_process_t *list_get_process(unsigned long pid) {

    for (_plist_entry_t *current = list_start; current->next != NULL; current = current->next)
        if (current->process_data->process_id == pid)
            return current->process_data;

    return NULL;
}

// bool list_remove_process(): Delete a process from the list (unschedules the process)
// inputs: pid - the process's id
// returns: true if removed, false if not removed or doesn't exist
bool list_remove_process(unsigned long pid) {

    if (list_start) {

        // search for the process in the list and delete it when found
        for (_plist_entry_t *current = list_start; current->next != NULL; current = current->next) {

            if (current->process_data->process_id == pid) {

                _plist_entry_t *prev = current->prev;
                _plist_entry_t *next = current->next;

                prev->next = next;
                next->prev = prev;

                kernel_free(current);
                return true;
            }
        }
    }

    return false;
}