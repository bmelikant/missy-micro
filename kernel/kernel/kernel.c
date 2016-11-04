/*
 * missy_microsystem/kernel/kernel/kernel.c
 *
 *	Main kernel file for MISSY Microsystem. This file was rewritten (once again) as part of the alpha 3.0 upgrade.
 *	The new system will be re-structured from the ground up (using some existing code) to incorporate process
 *	management, efficient memory management, and a new driver interface that is filesystem-based (similar to *nix
 *	type platforms.) Multiprocessing ability planned in this version
 *
 *  Created on: Aug 10, 2016
 *      Author: Ben Melikant
 *  Last Modified: Aug 14, 2016
 *
 *  Copyright (C) 2016 Ben Melikant
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// standard includes
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// kernel public includes (needed by / contained in libk)
#include <kernel/memory.h>
#include <sys/syscall.h>
#include <kernel/tty.h>

#include <include/exception.h>
#include <include/splashlogo.h>

// void kernel_main () : Main kernel execution method for MISSY Microsystem
// inputs: *mboot: Multiboot header information
// returns: should never return (-1 on error to ASM procedure)
void kernel_main () {

	// clear the screen and print our splash logo
	terminal_clrscr ();
	splash_logo ();

	// generate a random interrupt
	__asm__("int $0x02");


	for (;;);
	__builtin_unreachable ();
}

// int kernel_early_init (): Initialize the kernel (before calls to any C runtime)
// inputs: *mboot: Multiboot header information
// returns: -1 on error, 0 on success
int kernel_early_init (struct multiboot_info *mb_inf) {

	// initialize the terminal and print a short message
	terminal_initialize ();
	terminal_clrscr ();

	terminal_puts ("pre-boot terminal initialized...");
	terminal_puts ("running kernel_early_init ()...");

	// do early processor setup
	terminal_puts ("early processor setup in progress...");

	if (cpu_init () != 0)
		kernel_panic ("Failed to perform early processor setup!");

	// try to initialize physical memory manager
	terminal_puts ("calling kernel_setup_memory_manager ()...");
	kmemory_initialize (mb_inf);
	
	if (kspace_initialize () == -1)
		kernel_panic ("Kernel address space could not be initialized!");

	// everything is set up, return!
	return 0;
}
