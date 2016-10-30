/*
 * putchar.c: Place a character on the active terminal MISSY Microsystem Alpha 3.0
 *
 *	Created on 8/14/16 Ben Melikant
 *	Last Edit: 8/14/16
 */

#ifdef __is_missy_kernel
#include <kernel/vga_tty.h>
#endif

#include <stdio.h>

// int putchar (): Place a character on the display
// inputs: c - character to display
// returns: EOF on error, character value on success
int putchar (int c) {

	// if we are in the kernel, use the early terminal to print character
	#ifdef __is_missy_kernel
	terminal_printchar(c);
	return c;
	#endif

	// TODO: generate system call when NOT working in the kernel

	// default option: return error code
	return EOF;
}
