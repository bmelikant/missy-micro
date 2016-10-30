#ifndef _MISSY_KERNEL_DESCRIPTOR_H
#define _MISSY_KERNEL_DESCRIPTOR_H

#ifdef _cplusplus
extern "C" {
#endif

// constant definitions
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

// GDT function prototpyes
int 		i386_gdt_initialize ();
int			i386_gdt_setentry   (uint8_t index, uint32_t base_addr, uint32_t limit, uint16_t flags);
uint64_t 	i386_gdt_getentry	(uint8_t index);
int 		i386_gdt_clearentry (uint8_t index);

// IDT function prototypes
int 	i386_idt_initialize 	();
void 	i386_idt_setvector 	(uint8_t index, uint32_t addr, uint16_t codeSel, uint8_t attrib);

#ifdef _cplusplus
}
#endif

#endif
