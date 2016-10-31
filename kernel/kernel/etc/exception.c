/*
 * exception.c: handle panic situations on the kernel
 *
 *  Created on: Oct 31, 2016
 *      Author: bmmelikant
 */

// kernel panics halt the system
#include <device/cpu/cpu.h>

#include <kernel/tty.h>

// void kernel_panic (): pre-boot environment kernel panic
// inputs: err - error string
// returns: NEVER!!!
__attribute__((__noreturn__))
void kernel_panic (const char *err) {

	terminal_puts (err);
	terminal_puts ("Kernel must halt! Calling cpu_disable()...");

	cpu_disable ();

	terminal_puts ("Please reboot machine, or power off...");

	for (;;);
	__builtin_unreachable();
}
