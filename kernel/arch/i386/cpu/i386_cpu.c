/*
 * i386_cpu.c
 *
 *	x86-based CPU interface for MISSY Microsystem. This file defines all the functions needed
 *	for dealing with the processor at startup (initializing the descriptor tables, setting up
 *	segments and tss registers, possibly initializing paging, etc...)
 *
 *  Created on: Aug 10, 2016
 *      Author: Ben Melikant
 */

// standard includes
#ifndef _cplusplus
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>

// kernel private includes
#include <include/cpu.h>

// architecture includes
#include <i386/include/pic.h>
#include <i386/include/pit.h>

#define GDT_MAX_DESCRIPTORS 10
#define I386_MAX_INTERRUPTS 255

#define I386_GDT_DESCTYPE(x)	(x << 0x04)
#define I386_GDT_PRESENT(x)		(x << 0x07)
#define I386_GDT_AVAILABLE(x)	(x << 0x0c)
#define I386_GDT_LONGMODE(x)	(x << 0x0d)
#define I386_GDT_SIZE(x)		(x << 0x0e)
#define I386_GDT_GRANULARITY(x)	(x << 0x0f)
#define I386_GDT_PRIVILEGE(x)	((x & 0x03) << 0x05)

#define I386_GDT_DATA_RO			0x00
#define I386_GDT_DATA_RW			0x02
#define I386_GDT_DATA_READEXPAND	0x04
#define I386_GDT_DATA_RWEXPAND		0x06
#define I386_GDT_CODE_NOREAD		0x08
#define I386_GDT_CODE_TSS			0x09
#define I386_GDT_CODE_READ			0x0A
#define I386_GDT_CODE_NOREADCONFORM	0x0C
#define I386_GDT_CODE_READCONFORM	0x0E

#define GDT_CODE_RING_0		I386_GDT_DESCTYPE(1) | I386_GDT_PRESENT(1) | I386_GDT_AVAILABLE(0) | I386_GDT_LONGMODE(0) | \
				I386_GDT_SIZE(1) | I386_GDT_GRANULARITY(1) | I386_GDT_PRIVILEGE(0) | I386_GDT_CODE_READ
#define GDT_DATA_RING_0		I386_GDT_DESCTYPE(1) | I386_GDT_PRESENT(1) | I386_GDT_AVAILABLE(0) | I386_GDT_LONGMODE(0) | \
				I386_GDT_SIZE(1) | I386_GDT_GRANULARITY(1) | I386_GDT_PRIVILEGE(0) | I386_GDT_DATA_RW
#define GDT_CODE_RING_3		I386_GDT_DESCTYPE(1) | I386_GDT_PRESENT(1) | I386_GDT_AVAILABLE(0) | I386_GDT_LONGMODE(0) | \
				I386_GDT_SIZE(1) | I386_GDT_GRANULARITY(1) | I386_GDT_PRIVILEGE(3) | I386_GDT_CODE_READ
#define GDT_DATA_RING_3		I386_GDT_DESCTYPE(1) | I386_GDT_PRESENT(1) | I386_GDT_AVAILABLE(0) | I386_GDT_LONGMODE(0) | \
				I386_GDT_SIZE(1) | I386_GDT_GRANULARITY(1) | I386_GDT_PRIVILEGE(3) | I386_GDT_DATA_RW

#define I386_IDT_TASKGATE_32 		0x05
#define I386_IDT_INTERRUPTGATE_16	0x06
#define I386_IDT_TRAPGATE_16		0x07
#define I386_IDT_INTERRUPTGATE_32	0x0e
#define I386_IDT_TRAPGATE_32		0x0f

#define I386_IDT_PRESENT 	0x80
#define I386_IDT_RING0		0x00
#define I386_IDT_RING1   	0x20
#define I386_IDT_RING2		0x40
#define I386_IDT_RING3		0x60

// gdt register struct
struct gdtr {

	uint16_t gdt_sz;
	uint32_t gdt_ptr;

} __attribute__((packed));

// idtentry: A single entry in the Interrupt table
struct idtentry {

	uint16_t offsetLo;		// low 16 bits of int handler address
	uint16_t codeSelector;		// code selector (segment) of the int handler
	uint8_t  zero;			// reserved
	uint8_t  typeAttrib;		// type and attributes
	uint16_t offsetHi;		// upper part of int handler address

} __attribute__((packed));

// idtr: Represents the interrupt table register
struct idtr {

	uint16_t limit;		// size of the IDT
	uint32_t base;		// location of the IDT

} __attribute__((packed));

// internal variables
struct gdtr 	_gdtr;
uint64_t 		_gdt[GDT_MAX_DESCRIPTORS];
struct idtr     _idtr;
struct idtentry _idt[I386_MAX_INTERRUPTS];

// function prototypes
void 		i386_gdt_install 	(uint16_t, uint16_t);
int  		i386_gdt_setentry 	(uint8_t, uint32_t, uint32_t, uint16_t);
uint64_t 	i386_gdt_getentry 	(uint8_t);
int			i386_gdt_initialize ();

void 	i386_idt_install	();
void	i386_idt_setvector	(uint8_t idx, uint32_t addr, uint16_t codeSel, uint8_t flags);
int 	i386_idt_initialize	();

// exception handlers
extern void __attribute__((cdecl)) i386_defaulthandler		(void);
extern void __attribute__((cdecl)) i386_divbyzero 			(void);
extern void __attribute__((cdecl)) i386_debugtrap 			(void);
extern void __attribute__((cdecl)) i386_nmi 				(void);
extern void __attribute__((cdecl)) i386_breakpoint 			(void);
extern void __attribute__((cdecl)) i386_overflow 			(void);
extern void __attribute__((cdecl)) i386_bounderror 			(void);
extern void __attribute__((cdecl)) i386_badopcode 			(void);
extern void __attribute__((cdecl)) i386_deviceerror 		(void);
extern void __attribute__((cdecl)) i386_doublefault 		(void);
extern void __attribute__((cdecl)) i386_invalidtss 			(void);
extern void __attribute__((cdecl)) i386_segnotpresent 		(void);
extern void __attribute__((cdecl)) i386_stacksegfault 		(void);
extern void __attribute__((cdecl)) i386_genprotectfault 	(void);
extern void __attribute__((cdecl)) i386_pagefault 			(void);
extern void __attribute__((cdecl)) i386_fpuexception 		(void);
extern void __attribute__((cdecl)) i386_aligncheck 			(void);
extern void __attribute__((cdecl)) i386_machinecheck 		(void);
extern void __attribute__((cdecl)) i386_simdexception 		(void);
extern void __attribute__((cdecl)) i386_virtualizeexception (void);
extern void __attribute__((cdecl)) i386_securityexception 	(void);

/* Interface functions */

// int cpu_init (): Initialize the CPU
// inputs: none, returns: 0 on success, -1 on error
void cpu_init () {

	// make sure interrupts are disabled
	cpu_disable ();

	// initialize the descriptor tables and interrupt table
	i386_gdt_initialize ();
	i386_idt_initialize ();

	// install the interrupt handlers
	cpu_setvector (0x00, (void *) &i386_divbyzero);
	cpu_setvector (0x01, (void *) &i386_debugtrap);
	cpu_setvector (0x02, (void *) &i386_nmi);
	cpu_setvector (0x03, (void *) &i386_breakpoint);
	cpu_setvector (0x04, (void *) &i386_overflow);
	cpu_setvector (0x05, (void *) &i386_bounderror);
	cpu_setvector (0x06, (void *) &i386_badopcode);
	cpu_setvector (0x07, (void *) &i386_deviceerror);
	cpu_setvector (0x08, (void *) &i386_doublefault);
	cpu_setvector (0x0A, (void *) &i386_invalidtss);
	cpu_setvector (0x0B, (void *) &i386_segnotpresent);
	cpu_setvector (0x0C, (void *) &i386_stacksegfault);
	cpu_setvector (0x0D, (void *) &i386_genprotectfault);
	cpu_setvector (0x0E, (void *) &i386_pagefault);
	cpu_setvector (0x10, (void *) &i386_fpuexception);
	cpu_setvector (0x11, (void *) &i386_aligncheck);
	cpu_setvector (0x12, (void *) &i386_machinecheck);
	cpu_setvector (0x13, (void *) &i386_simdexception);
	cpu_setvector (0x14, (void *) &i386_virtualizeexception);
	cpu_setvector (0x1E, (void *) &i386_securityexception);

    // set up the interrupt controller and mask the timer
    i386_pic_initialize (0x20, 0x28);
    //i386_pic_mask_irq (I386_PIC_IRQ0);

    // set up the interval timer
    i386_pit_initialize ();

    // now that the CPU is all set up along with its necessary
    // devices, we can go ahead and start interrupts!!
    cpu_enable ();

    // print a message: we are loaded into the cpu!
    terminal_puts ("cpu_init () success.");
}

// int cpu_shutdown (): Shut down the CPU. This function disables interrupts to halt the system
// inputs: none, returns: 0
int cpu_shutdown () {

	// disable interrupts on the processor
	cpu_disable ();

	return 0;
}

// int cpu_save_state (): save the current state of the CPU
// inputs: ps - state of the processor
// returns: 0 on success, nonzero on error (sets errno)
int cpu_save_state () {

	// save the processor registers onto the stack

	return 0;
}

// void cpu_reset (): load an invalid IDT and execute an interrupt
// Inputs: None
// Returns: None
void cpu_reset () {

	// disable interrupts while modifying the table
	cpu_disable ();

	// set an invalid descriptor then call it
	cpu_setvector (0x80, (void *) 0x00);

	// execute an invalid interrupt
	cpu_interrupt (0x80);
}

// void cpu_setvector (): Set an interrupt vector up
void cpu_setvector (int vector, void *addr) {

	terminal_printf ("Setting cpu vector 0x%x\n", (uint8_t)(vector));
	i386_idt_setvector ((uint8_t) vector, (uint32_t)(addr), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
}

void cpu_enable_irq (int irq) { i386_pic_unmask_irq ((uint8_t) irq); }
void cpu_disable_irq (int irq) { i386_pic_mask_irq ((uint8_t) irq); }

// i386_gdt_install (): Install the GDT into the GDTR
void i386_gdt_install (uint16_t codeseg, uint16_t dataseg) {

	// place the updated _gdt into the gdt register
	asm volatile ("lgdt (%0)" :: "m" (_gdtr));
	asm volatile ("\n"
		"pushl %0\n"
		"pushl $1f\n"
		"lret\n"
		"1:\n"
		"mov %1,%%eax\n"
		"mov %%ax,%%ds\n"
		"mov %%ax,%%es\n"
		"mov %%ax,%%fs\n"
		"mov %%ax,%%gs\n"
		"mov %%ax,%%ss\n"
		:: "g" (codeseg), "g" (dataseg) : "eax");
}

// i386_gdt_setentry (): Set up a descriptor entry in the GDT
int i386_gdt_setentry (uint8_t idx, uint32_t base, uint32_t limit, uint16_t flags) {

	// check for index out of bounds
	if (idx > GDT_MAX_DESCRIPTORS)
		return -1;

	// zero out the descriptor
	_gdt[idx] = 0x0;

	// now set up the descriptor in the table
	_gdt[idx]  = limit        & 0x000f0000;
	_gdt[idx] |= (flags << 8) & 0x00f0ff00;
	_gdt[idx] |= (base >> 16) & 0x000000ff;
	_gdt[idx] |= base         & 0xff000000;

	_gdt[idx] <<= 32;		// shift the descriptor right by 32 bits

	// create the low 32 bits of the descriptor
	_gdt[idx] |= base << 16;
	_gdt[idx] |= limit & 0x0000ffff;

	return 0;
}

uint64_t i386_gdt_getentry (uint8_t idx) {

	if (idx < GDT_MAX_DESCRIPTORS)
		return _gdt[idx];

	return 0;
}

// i386_gdt_initialize: Initialize the x86 global descriptor table
int i386_gdt_initialize () {

	// zero out the memory for the GDT
	memset (&_gdt, 0, sizeof(uint64_t)*GDT_MAX_DESCRIPTORS-1);

	// set up the descriptors
	i386_gdt_setentry (0,0,0,0);
	i386_gdt_setentry (1,0x0,0x000fffff,GDT_CODE_RING_0);
	i386_gdt_setentry (2,0x0,0x000fffff,GDT_DATA_RING_0);

	// set up the gdt table register entry
	_gdtr.gdt_sz  = (sizeof(uint64_t)*GDT_MAX_DESCRIPTORS)-1;
	_gdtr.gdt_ptr = (uint32_t)&_gdt[0];

	// install the global descriptor table
	// gdt_flush ();
	i386_gdt_install (0x08, 0x10);
	return 0;
}

// void i386_idt_install (): Puts the variable _idtr into the IDT register
// Inputs: None
// Returns: None
void i386_idt_install () {

	__asm__ ("lidt (%0)" :: "m" (_idtr));
}

// Interface Functions

// i386_idt_setvector (uint32_t, uint8_t): Point the requested interrupt at a new function
// Inputs: addr: The address of the interrupt vector, codesel: Code selector, idx: interrupt number
// Returns: None
void i386_idt_setvector (uint8_t idx, uint32_t addr, uint16_t codesel, uint8_t attr) {

    // clear the current descriptor
    memset ((void*)(&_idt[idx]),0,sizeof(struct idtentry));

	_idt[idx].codeSelector = codesel;
	_idt[idx].zero         = 0;
	_idt[idx].typeAttrib   = attr;
	_idt[idx].offsetLo     = (uint16_t) (addr &~ 0xffff0000);
	_idt[idx].offsetHi     = (uint16_t) (addr >> 16);
}

// int i386_idt_initialize (): Initialize the interrupt descriptor table
int i386_idt_initialize () {

    // set the memory block at the current interrupt to null
	memset ((void*)(&_idt[0]),0,sizeof(struct idtentry)*I386_MAX_INTERRUPTS-1);

	for (int i = 0; i < I386_MAX_INTERRUPTS; i++)
		i386_idt_setvector (i, (uint32_t)(&i386_defaulthandler), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));

	_idtr.limit = (sizeof(struct idtentry)*I386_MAX_INTERRUPTS)-1;
	_idtr.base  = (uint32_t)&_idt[0];

	i386_idt_install ();
	return 0;
}
