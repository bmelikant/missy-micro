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
#include <kernel/tty.h>
#include <kernel/device.h>
#include <kernel/process.h>

// kernel private includes
#include <include/multiboot.h>
#include <include/acpi.h>
#include <include/exception.h>
#include <include/splashlogo.h>
#include <include/cpu.h>
#include <include/kmemory.h>

#include <i386/include/ps2_kb.h>

// the command line function
int kernel_command_line();

// void kernel_main () : Main kernel execution method for MISSY Microsystem
// inputs: *mboot: Multiboot header information
// returns: should never return (-1 on error to ASM procedure)
void kernel_main () {

	// clear the screen and print our splash logo
	terminal_clrscr ();
	splash_logo ();

	terminal_puts ("MISSY v3.0 initialized successfully. Loading...");

	// try to initialize the boot time tty
	if (kernel_tty_initialize () == -1)
		kernel_panic ("Could not initialize the boot time TTY device!");

	// run the basic command line
	for (;;) {
		kernel_command_line();
	}

	cpu_disable();
	for (;;);

	__builtin_unreachable ();
}

// int kernel_early_init (): Initialize the kernel (before calls to any C runtime)
// inputs: *mboot: Multiboot header information (supports multiboot2 as of last commit to master)
// returns: -1 on error, 0 on success
int kernel_early_init (void *mb_inf, unsigned int mboot_magic) {

	// initialize the terminal and print a short message
	terminal_initialize ();
	terminal_clrscr ();

	terminal_puts ("pre-boot terminal initialized...");
	terminal_puts ("running kernel_early_init ()...");

	// first thing: load the multiboot system
	if (multiboot_data_init (mb_inf, mboot_magic) == -1)
		kernel_panic ("No usable multiboot information was found!");

	terminal_printf ("Multiboot reports physical memory of %d kilobytes\n", multiboot_get_memsz());

	// early processor and memory initialization
	cpu_init ();
	kmemory_initialize ();
	kspace_initialize  ();
	device_list_init   ();

	// try to locate the tables
	RSDP_Descriptor *acpi_table = acpi_locate_rsdp ();

	if (!acpi_table)
		kernel_panic ("Could not locate the ACPI tables!");

	// check and report the ACPI compliance version
	terminal_printf ("Found ACPI data for ACPI version 0x%x\n", acpi_table->revision);

	if (acpi_get_checksum (acpi_table) > 0)
		kernel_panic ("The ACPI table is invalid!");

	// if the RSDT is in identity-mapped memory we are okay
	// otherwise, we have to map it into memory
	//terminal_printf ("The RSDT is located at physical address 0x%x\n", (unsigned int) acpi_table->rsdt_addr);

	// try to locate the table and print its physical address
	//void *fadt = locate_acpi_table("FACP", (void *) acpi_table);
	//terminal_printf ("This is where the FADT was reported to be: 0x%x\n", (unsigned int) fadt);

	// this is to try to allocate a new process!
	unsigned long pid = create_user_process(NULL);
	terminal_printf("I spawned a new process with pid=0x%x\n", (unsigned int) pid);

	// acpi build stop point
	cpu_disable();
	for (;;);

	// everything is set up, return!
	return 0;
}

// hold kernel terminal builtin commands
typedef struct KTERMINAL_BUILTIN {

	unsigned char *commandtxt;
	int (*commandcode)(void);

} kterm_builtin;

int kterminal_help();
int kterminal_clear();
int kterminal_ticks();

// the number of builtins we have
kterm_builtin_count = 3;

// the actual array of builtins
kterm_builtin builtins[] = {
	{ "help", kterminal_help },
	{ "clear", kterminal_clear },
	{ "ticks", kterminal_ticks }
};

// run a single command from the builtin terminal
int kernel_command_line () {

	// output the prompt
	terminal_printf("prompt:> ");

	// allocate space on the stack and get the input string
	unsigned char *cmdstr = kernel_alloc(255);
	memset(cmdstr, 0, 255);
	kernel_tty_read(cmdstr, 255);
	terminal_puts("");

	// check to see if the command is a builtin
	for (size_t i = 0; i < kterm_builtin_count; i++)
		if (!strncmp(cmdstr, builtins[i].commandtxt, strlen(cmdstr)))
			return builtins[i].commandcode();

	// otherwise, unknown command
	terminal_puts("Error: unrecognized command");
	return -1;
}

// kterminal_help (): Print the help menu for the kernel
int kterminal_help () {
	
		terminal_puts("\nKernel TTY built-in commands:\n");
		terminal_puts("\tclear: Clears the input terminal");
		terminal_puts("\tticks: print the current PIT tick count");
		terminal_puts("");

		return 0;
}

// kterminal_clear(): Clear the display
int kterminal_clear() {

	terminal_clrscr();
	return 0;
}

extern int pit_tick_count;

// print the current number of ticks
int kterminal_ticks() {

	terminal_printf("Current tick count: %d\n", pit_tick_count);
	return 0;
}