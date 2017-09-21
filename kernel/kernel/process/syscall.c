/*
 * syscall.c: System call handler for MISSY Microsystem. Installs system call service
 * and handles incoming requests for system calls.
 *
 *  Created on: Oct 27, 2016
 *      Author: bmelikant
 */

#include <sys/syscall.h>

// kernel private includes
#include <include/kmemory.h>
#include <include/cpu.h>

#define SYSCALL_MAX 256
typedef long (*system_call)(void);

// system call table
system_call syscall_table[SYSCALL_MAX];

// void install_syscall (): Install a system call into the table
// inputs: idx - system call number, syscall - address of system call
// returns: none
void install_syscall (unsigned int idx, system_call c) { syscall_table[idx] = c; }

//
