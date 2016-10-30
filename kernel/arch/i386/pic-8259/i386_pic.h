#ifndef I386_PIC_H
#define I386_PIC_H

/*
 *      i386_pic.h: Header file to declare PIC functions for BenOS v0.1
 *      Written by Ben Melikant 10/2014
 *      Code adapted from OSDev Wiki and Brokenthorn's OS Development series 
 */

// standard includes

#if !defined(_cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

// constant definitions

// IRQ line masks

#define I386_PIC_IRQ0            0x0001
#define I386_PIC_IRQ1            0x0002
#define I386_PIC_IRQ2            0x0004
#define I386_PIC_IRQ3            0x0008
#define I386_PIC_IRQ4            0x0010
#define I386_PIC_IRQ5            0x0020
#define I386_PIC_IRQ6            0x0040
#define I386_PIC_IRQ7            0x0080
#define I386_PIC_IRQ8            0x0001
#define I386_PIC_IRQ9            0x0002
#define I386_PIC_IRQ10           0x0004
#define I386_PIC_IRQ11           0x0008
#define I386_PIC_IRQ12           0x0010
#define I386_PIC_IRQ13           0x0020
#define I386_PIC_IRQ14           0x0040
#define I386_PIC_IRQ15           0x0080

#define i386_interrupt_enable()  __asm__ volatile("sti");
#define i386_interrupt_disable() __asm__ volatile("cli");

// Interface Functions

int             i386_pic_initialize         (uint8_t, uint8_t);
void            i386_pic_enable             ();
void            i386_pic_disable            ();
void            i386_pic_mask_irq           (uint8_t irq);
void            i386_pic_unmask_irq         (uint8_t irq);
uint32_t        i386_pic_spurious_count     ();

#endif
