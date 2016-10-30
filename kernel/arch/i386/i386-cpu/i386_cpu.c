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

// include local headers
#include "../i386-cpu/i386_cpu.h"
#include "../i386-cpu/i386_fpu.h"
#include "../i386-cpu/i386_descriptor.h"
#include "../pic-8259/i386_pic.h"

// internal variables
struct gdtr 	_gdtr;
uint64_t 		_gdt[GDT_MAX_DESCRIPTORS];
struct idtr     _idtr;
struct idtentry _idt[I386_MAX_INTERRUPTS];

// external functions
extern void __attribute__((cdecl)) x86_default_interrupt (void);

// interrupt exception handlers
extern void __attribute__((cdecl)) x86_dividebyzero             (void);
extern void __attribute__((cdecl)) x86_debugtrap                (void);
extern void __attribute__((cdecl)) x86_nonmaskable              (void);
extern void __attribute__((cdecl)) x86_breakpoint               (void);
extern void __attribute__((cdecl)) x86_overflow                 (void);
extern void __attribute__((cdecl)) x86_bounderror               (void);
extern void __attribute__((cdecl)) x86_badopcode                (void);
extern void __attribute__((cdecl)) x86_deviceerror              (void);
extern void __attribute__((cdecl)) x86_doublefault              (void);
extern void __attribute__((cdecl)) x86_invalidtss               (void);
extern void __attribute__((cdecl)) x86_segnotpresent            (void);
extern void __attribute__((cdecl)) x86_stacksegfault            (void);
extern void __attribute__((cdecl)) x86_genprotectfault          (void);
extern void __attribute__((cdecl)) x86_pagefault                (void);
extern void __attribute__((cdecl)) x86_fpuexception             (void);
extern void __attribute__((cdecl)) x86_aligncheck               (void);
extern void __attribute__((cdecl)) x86_machinecheck             (void);
extern void __attribute__((cdecl)) x86_simdexception            (void);
extern void __attribute__((cdecl)) x86_virtualizeexception      (void);
extern void __attribute__((cdecl)) x86_securityexception        (void);

/* Interface functions */

// int cpu_init (): Initialize the CPU
// inputs: none, returns: 0 on success, -1 on error
int cpu_init () {

	// initialize the descriptor tables and interrupt table
	i386_gdt_initialize ();
	i386_idt_initialize ();

	// set up the exception handlers
	i386_idt_setvector (0x00, (uint32_t)(&x86_dividebyzero), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x01, (uint32_t)(&x86_debugtrap), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x02, (uint32_t)(&x86_nonmaskable), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x03, (uint32_t)(&x86_breakpoint), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x04, (uint32_t)(&x86_overflow), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x05, (uint32_t)(&x86_bounderror), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x06, (uint32_t)(&x86_badopcode), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x07, (uint32_t)(&x86_deviceerror), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x08, (uint32_t)(&x86_doublefault), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x0A, (uint32_t)(&x86_invalidtss), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x0B, (uint32_t)(&x86_segnotpresent), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
   	i386_idt_setvector (0x0C, (uint32_t)(&x86_stacksegfault), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x0D, (uint32_t)(&x86_genprotectfault), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x0E, (uint32_t)(&x86_pagefault), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x10, (uint32_t)(&x86_fpuexception), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
   	i386_idt_setvector (0x11, (uint32_t)(&x86_aligncheck), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x12, (uint32_t)(&x86_machinecheck), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x13, (uint32_t)(&x86_simdexception), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x14, (uint32_t)(&x86_virtualizeexception), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));
    i386_idt_setvector (0x1E, (uint32_t)(&x86_securityexception), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));

    // set up the interrupt controller and timer
    i386_pic_initialize (0x20, 0x28);

    // try to start the floating point unit

    /*int init_result = i386_fpu_initialize();

	if (init_result != I386_FPU_SUCCESS)
		return init_result;
    */

    // now that the CPU is all set up along with its necessary
    // devices, we can go ahead and start interrupts!!
    cpu_enable ();

	return 0;
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
int cpu_save_state (cpu_state *ps) {

	// save the processor registers into the structure

	// general registers
	__asm__ ("movl %%eax,%0\n" : "=r"(ps->eax));
	__asm__ ("movl %%ebx,%0\n" : "=r"(ps->ebx));
	__asm__ ("movl %%ecx,%0\n" : "=r"(ps->ecx));
	__asm__ ("movl %%edx,%0\n" : "=r"(ps->edx));

	// segment registers
	__asm__ ("movl %%cs,%0\n" : "=r"(ps->cs));
	__asm__ ("movl %%ds,%0\n" : "=r"(ps->ds));
	__asm__ ("movl %%es,%0\n" : "=r"(ps->es));
	__asm__ ("movl %%fs,%0\n" : "=r"(ps->fs));
	__asm__ ("movl %%gs,%0\n" : "=r"(ps->gs));
	__asm__ ("movl %%ss,%0\n" : "=r"(ps->ss));



	return 0;
}

// void cpu_reset (): load an invalid IDT and execute an interrupt
// Inputs: None
// Returns: None
void cpu_reset () {

	// disable interrupts while modifying the table
	cpu_disable ();

	// set an invalid descriptor then call it
	i386_idt_setvector (0x80, 0, 0, 0);

	// execute an invalid interrupt
	cpu_interrupt (0x80);
}

// void cpu_setvector (): Set an interrupt vector up
void cpu_setvector (int vector, void *addr) {

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
		i386_idt_setvector (i, (uint32_t)(&x86_default_interrupt), 0x08, (uint8_t) (I386_IDT_INTERRUPTGATE_32 | I386_IDT_PRESENT | I386_IDT_RING0));

	_idtr.limit = (sizeof(struct idtentry)*I386_MAX_INTERRUPTS)-1;
	_idtr.base  = (uint32_t)&_idt[0];

	i386_idt_install ();
	return 0;
}
