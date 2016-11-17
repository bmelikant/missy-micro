/*
 * cpu.h
 *
 *	Hardware-independent interface to the microprocessor. Allows startup of
 *  Created on: Aug 10, 2016
 *      Author: Ben Melikant
 */

#ifndef _MISSY_KERNEL_CPU_H
#define _MISSY_KERNEL_CPU_H

// standard headers
#ifndef _cplusplus
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

// architecture-specific functions and types
#ifdef __arch_i386

// CPU interface functions

#define cpu_interrupt(x)	__asm__("int %0\n" :: "N"((x)) : "cc", "memory")
#define cpu_enable()  		__asm__("sti")
#define cpu_disable() 		__asm__("cli")

// port input functions
static inline uint8_t inportb (uint16_t port) {

	uint8_t ret;
	__asm__ volatile ("inb %1,%0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static inline uint16_t inportw (uint16_t port) {

	uint16_t ret;
	__asm__ volatile ("inw %1,%0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static inline uint32_t inportd (uint16_t port) {

	uint16_t ret;
	__asm__ volatile ("inl %1,%0" : "=a"(ret) : "Nd"(port));
	return ret;
}

// port output functions
static inline void outportb (uint16_t port, uint8_t byte)   { __asm__ volatile ("outb %0,%1" :: "a"(byte), "Nd"(port));  }
static inline void outportw (uint16_t port, uint16_t word)  { __asm__ volatile ("outw %0,%1" :: "a"(word), "Nd"(port));  }
static inline void outportd (uint16_t port, uint32_t dword) { __asm__ volatile ("outw %0,%1" :: "a"(dword), "Nd"(port)); }

#endif

/* interface routines, defined in the architecture-specific CPU files */

void 	cpu_init 			();								// initialize the CPU for use
int 	cpu_shutdown 		();								// shut down the CPU interface (should disable interrupts to halt the CPU)
int		cpu_save_state		();								// save the state of the processor onto the current stack
int		cpu_restore_state	();								// restore the state of the processor from the current stack
void 	cpu_reset			();								// reset the system
void  	cpu_setvector		(int vector, void *addr);		// set a CPU interrupt to point to a new function

void cpu_enable_irq 	(int irq);
void cpu_disable_irq 	(int irq);

#endif /* KERNEL_CPU_CPU_H_ */
