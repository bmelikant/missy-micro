/*
 * i386_cpu.h: i386-specific CPU information (structures, etc...)
 *
 *  Created on: Aug 18, 2016
 *      Author: bmelikant
 */

#ifndef MISSY_KERNEL_I386_CPU_H_
#define MISSY_KERNEL_I386_CPU_H_

#include <stdint.h>
#include <stddef.h>

// processor state struct
typedef struct PROCESSOR_STATE {

	uint32_t eax, ebx, ecx, edx;		// general registers
	uint32_t cs, ds, es, fs, gs, ss;	// segment registers
	uint32_t esi, edi;					// string registers
	uint32_t esp, ebp;					// stack registers
	uint32_t eflags;					// flags register
	uint32_t cr0, cr2, cr3, cr4;		// control registers

} cpu_state;

// CPU interface functions

int 	cpu_init 			();								// initialize the CPU for use
int 	cpu_shutdown 		();								// shut down the CPU interface (should disable interrupts to halt the CPU)
int		cpu_save_state		(cpu_state *ps);				// save the state of the processor
int		cpu_restore_state	(cpu_state *ps);				// restore the state of the processor from the given struct
void 	cpu_reset			();								// reset the system
void  	cpu_setvector		(int vector, void *addr);		// set a CPU interrupt to point to a new function

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

#endif /* MISSY_KERNEL_I386_CPU_H_ */
