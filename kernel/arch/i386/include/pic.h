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
#define I386_PIC_IRQ8            I386_PIC_IRQ0
#define I386_PIC_IRQ9            I386_PIC_IRQ1
#define I386_PIC_IRQ10           I386_PIC_IRQ2
#define I386_PIC_IRQ11           I386_PIC_IRQ3
#define I386_PIC_IRQ12           I386_PIC_IRQ4
#define I386_PIC_IRQ13           I386_PIC_IRQ5
#define I386_PIC_IRQ14           I386_PIC_IRQ6
#define I386_PIC_IRQ15           I386_PIC_IRQ7

// Interface Functions

int             i386_pic_initialize         (uint8_t, uint8_t);
void            i386_pic_enable             ();
void            i386_pic_disable            ();
void            i386_pic_mask_irq           (uint8_t irq);
void            i386_pic_unmask_irq         (uint8_t irq);
uint32_t        i386_pic_spurious_count     ();

#endif
