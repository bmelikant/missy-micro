/*
 * timer-8254.c: Minidriver for Intel 8254 PIT chip.
 * Works through the kernel's device layer timer interface
 *
 *  Created on: Oct 30, 2016
 *      Author: bmmelikant
 */

#include <errno.h>
#include <stdio.h>

#include <device/device.h>
#include <device/cpu/cpu.h>
#include <device/timer/timer.h>

#include <stdint.h>

#define PIT_IRQ 0x0001

#define PIT_PORT_COUNTER0		0x40
#define PIT_PORT_COUNTER1		0x41
#define PIT_PORT_COUNTER2		0x42
#define PIT_PORT_COMMAND		0x43

#define PIT_MODE_BINARY			0x00
#define PIT_MODE_BCD			0x01

#define PIT_OPERATION_TC			0x00
#define PIT_OPERATION_ONESHOT		0x02
#define PIT_OPERATION_RATEGEN		0x04
#define PIT_OPERATION_SQUAREWAVE	0x06
#define PIT_OPERATION_SOFTSTROBE	0x08
#define PIT_OPERATION_HARDSTROBE	0x0A

#define PIT_LOAD_LSBONLY	0x10
#define PIT_LOAD_MSBONLY	0x20
#define PIT_LOAD_LSBFIRST	0x30

#define PIT_COUNTER_0		0x00
#define PIT_COUNTER_1		0x40
#define PIT_COUNTER_2		0x80

// function declarations
ssize_t timer_read_device (char *buf, size_t length);

// external (asm) data
extern void __attribute__((cdecl)) timer_8254_irq (void);
extern uint32_t timer_8254_count;

// internal driver data
static int timer_8254_major = 0;
const char *timer_8254_name = "timer-8254";

chrdev_ops timer_8254_ops = {
		.read_chrdev = timer_read_device,
};

// int timer_initialize (): Initialize the 8254 timer
// inputs: none
// returns: 0 on success, -1 on error
int timer_initialize (void) {

	// hold interrupts on the CPU for now
	cpu_disable ();

	// install the timer interrupt handler
	cpu_setvector (0x20, (void *)(&timer_8254_irq));
	cpu_enable_irq (PIT_IRQ);

	// register this device
	timer_8254_major = register_chrdev (0, timer_8254_name, &timer_8254_ops);

	// we can now restore interrupts
	cpu_enable ();
	return timer_8254_major;
}

// int timer_start (): Start a new timer a-runnin!!
// inputs: freq - frequency of timer (in Hz)
// returns: none
int timer_start (int freq) {

	// disable the irq for a few seconds
	cpu_disable_irq (PIT_IRQ);

	uint16_t interval = (uint16_t) (1193182 / freq);

	// send initialization commands to the PIT
	outportb (PIT_PORT_COMMAND, PIT_COUNTER_0 | PIT_LOAD_LSBFIRST | PIT_OPERATION_SQUAREWAVE | PIT_MODE_BINARY);

	// send the counter frequency to the PIT
	outportb (PIT_PORT_COUNTER0, (uint8_t)(interval & 0xff));
	outportb (PIT_PORT_COUNTER0, (uint8_t)((interval >> 8) & 0xff));

	// set the counter to zero
	timer_8254_count = 0;

	// re-enable the irq
	cpu_enable_irq (PIT_IRQ);
	return 0;
}

// ssize_t timer_read_device (): Read from the timer
// inputs: buf - buffer to fill, length - size of buffer
// returns: # of bytes read (returns -1 and sets errno on failure)
ssize_t timer_read_device (char *buf, size_t length) {

	// at this point just send back the timer value
	if (length >= sizeof (unsigned int)) {

		unsigned int *buffer = (unsigned int *) buf;
		*buffer = (unsigned int) timer_8254_count;

		return sizeof(unsigned int);
	}

	// the buffer isn't big enough!
	errno = ENOBUFS;
	return -1;
}

// ssize_t timer_write_device (): Write commands to the timer
// inputs: buf - command buffer, length - size of buffer
// returns: # of bytes accepted (returns -1 and sets errno on failure)
