/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      i386_pic.c: Functions to program the Programmable Interrupt Controller for BenOS v0.1   *
 *      Written by Benjamin Melikant 10/2014                                                   *
 *      Code adapted from OSDev Wiki and Brokenthorn Entertainment's OS Development Series     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// includes

// architecture includes
#include <i386/include/pic.h>

// kernel private includes
#include <include/cpu.h>

// Constant definitions

#define I386_PIC1_PORT_COMMAND   0x20
#define I386_PIC1_PORT_DATA      0x21
#define I386_PIC2_PORT_COMMAND   0xA0
#define I386_PIC2_PORT_DATA      0xA1

#define I386_PIC_COMMAND_EOI             0x20
#define I386_PIC_COMMAND_READIRR         0x0A
#define I386_PIC_COMMAND_READISR         0x0B

// Initialization Command Word 1

#define I386_PIC_ICW1_SEND4              0x01            // sent if Initialization Command Word 4 will be sent
#define I386_PIC_ICW1_SINGLE             0x02            // sent if the system has only one PIC
#define I386_PIC_ICW1_INTERVAL4          0x04            // call address interval. 16-bit addresses if this is set
#define I386_PIC_ICW1_LEVELTRIGGER       0x08            // if set, operates in level-triggered mode. Otherwise, edge triggered
#define I386_PIC_ICW1_INITIALIZE         0x10            // if set, the PIC should wait for the next 2 (or 3) words

// Initialization Command Word 4

#define I386_PIC_ICW4_8086               0x01            // the PIC should operate in 8086 mode
#define I386_PIC_ICW4_AUTOEOI            0x02            // automatically generate EOI signal
#define I386_PIC_ICW4_SLAVEBUFFER        0x04            // slave PIC operates in buffered mode
#define I386_PIC_ICW4_MASTERBUFFER       0x08            // master PIC operates in buffered mode
#define I386_PIC_ICW4_SFNM               0x10            // PIC operates in Special fully nested mode

#define i386_pic_isrset(x,y) ((x)&(1<<(y)))

// function declarations

uint16_t         i386_pic_get_irqreg      (uint8_t ocw3);

extern void __attribute__((cdecl)) i386_handler_irq7 (void);
extern void __attribute__((cdecl)) i386_handler_irq15 (void);

// internal data

uint32_t spurious = 0;

// Interface Functions

// int i386_pic_initialize (): Initialize the Programmable Interrupt Controller
// Inputs: None
// Returns: 0
int i386_pic_initialize (uint8_t pic1_off, uint8_t pic2_off) {

        // save the two PIC bitmask registers
        uint8_t mask_one = inportb (I386_PIC1_PORT_DATA);
        uint8_t mask_two = inportb (I386_PIC2_PORT_DATA);

        // tell the PIC we are going to reinitialize it

        outportb (I386_PIC1_PORT_COMMAND, I386_PIC_ICW1_INITIALIZE | I386_PIC_ICW1_SEND4);
        outportb (I386_PIC2_PORT_COMMAND, I386_PIC_ICW1_INITIALIZE | I386_PIC_ICW1_SEND4);

        // send the interrupt offsets to the PICs

        outportb (I386_PIC1_PORT_DATA, pic1_off);
        outportb (I386_PIC2_PORT_DATA, pic2_off);

        // cascade commands

        outportb (I386_PIC1_PORT_DATA, 0x04);
        outportb (I386_PIC2_PORT_DATA, 0x02);

        // we want them to operate in 8086 mode

        outportb (I386_PIC1_PORT_DATA, I386_PIC_ICW4_8086);
        outportb (I386_PIC2_PORT_DATA, I386_PIC_ICW4_8086);

        // return the interrupt masks to their original state
        outportb (I386_PIC1_PORT_DATA, mask_one);
        outportb (I386_PIC2_PORT_DATA, mask_two);

        // install the two spurious interrupt handlers
        cpu_setvector (0x27, &i386_handler_irq7);
        cpu_setvector (0x2f, &i386_handler_irq15);
        
        return 0;
}

// void i386_pic_mask_irq (): Mask (disable) the specified interrupt request line
// Inputs: irq: The IRQ to disable
// Returns: None
void i386_pic_mask_irq (uint8_t irq) {

        uint16_t port;
        uint8_t  irq_mask;

        if (irq < 8)
                port = I386_PIC1_PORT_DATA;
        else {

                port = I386_PIC2_PORT_DATA;
                irq -= 8;
        }

        irq_mask = inportb (port) | (1 << irq);
        outportb (port, irq_mask);
}

// void i386_pic_unmask_irq (): Unmask (enable) the specified interrupt request line
// Inputs: irq: The IRQ to enable
// Returns: None
void i386_pic_unmask_irq (uint8_t irq) {

        uint16_t port;
        uint8_t  irq_mask;

        if (irq < 8)
                port = I386_PIC1_PORT_DATA;
        else {

                port = I386_PIC2_PORT_DATA;
                irq -= 8;
        }

        irq_mask = inportb (port) &~ (1 << irq);
        outportb (port, irq_mask);
}

// void i386_pic_enable (): Enable the programmable interrupt controllers
// Inputs: None
// Returns: None
void i386_pic_enable () {

        outportb (I386_PIC1_PORT_DATA, 0x00);
        outportb (I386_PIC2_PORT_DATA, 0x00);
}

// void i386_pic_disable (): Disable the programmable interrupt controllers
// Inputs: None
// Returns: None
void i386_pic_disable () {

        outportb (I386_PIC1_PORT_DATA, 0xff);
        outportb (I386_PIC2_PORT_DATA, 0xff);
}

// void i386_pic_eoi (): Send end-of-interrupt signal to the interrupt controllers
// Inputs: None
// Returns: None
void i386_pic_send_eoi (uint8_t irq) {

        // make sure we don't have a spurious interrupt
        uint16_t irqreg = i386_pic_get_irqreg (I386_PIC_COMMAND_READISR);
        
        if (irq == 7 || irq == 15) {

                if (!i386_pic_isrset(irqreg,irq)) {

                        // if the IRQ is 15, we still have to send EOI to the master PIC
                        if (irq == 15)
                                outportb (I386_PIC1_PORT_COMMAND, I386_PIC_COMMAND_EOI);

                        spurious++;
                        return;
                }
        }

        if (irq >= 8)
                outportb (I386_PIC2_PORT_COMMAND, I386_PIC_COMMAND_EOI);

         outportb (I386_PIC1_PORT_COMMAND, I386_PIC_COMMAND_EOI);
}

// uint32_t i386_pic_spurious_count (): Get the number of spurious interrupts
// Inputs: None
// Returns: Spurious interrupt count
uint32_t i386_pic_spurious_count () { return spurious; }

// Internal functions

// static uint16_t i386_pic_get_irqreg (): Read the current status of the PIC interrupt registers
// Inputs: ocw3: command to send (which register to read)
// Returns: the current irq register value for both PICs (hibyte: pic2, lobyte: pic1)
uint16_t i386_pic_get_irqreg (uint8_t ocw3) {

        outportb (I386_PIC1_PORT_COMMAND, ocw3);
        outportb (I386_PIC2_PORT_COMMAND, ocw3);

        return ((uint16_t)(inportb (I386_PIC2_PORT_DATA) << 8 | inportb(I386_PIC1_PORT_DATA)));
}
