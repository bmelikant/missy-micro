/*
 * pit.c
 *
 *  Created on: Mar 5, 2017
 *      Author: bmelikant
 */

#include <include/cpu.h>

#include <i386/include/pic.h>
#include <i386/include/pit.h>

#define PIT_CHANNEL0_DATA_PORT 	0x40
#define PIT_CHANNEL1_DATA_PORT 	0x41
#define PIT_CHANNEL2_DATA_PORT 	0x42
#define PIT_COMMAND_PORT		0x43

#define PIT_CMD_CHANNEL_0		0x00
#define PIT_CMD_CHANNEL_1		0x40
#define PIT_CMD_CHANNEL_2		0x80
#define PIT_READBACK_COMMAND	0xc0

#define PIT_CMD_LATCH_COUNT		0x00
#define PIT_CMD_LOBYTE_ONLY		0x10
#define PIT_CMD_HIBYTE_ONLY		0x20
#define PIT_CMD_LOBYTE_FIRST	0x30

#define PIT_CMD_IOTC		0x00
#define PIT_CMD_ONESHOT 	0x02
#define PIT_CMD_RATEGEN		0x04
#define PIT_CMD_SQUAREWAVE	0x06
#define PIT_CMD_SW_STROBE	0x08
#define PIT_CMD_HW_STROBE	0x0a

#define PIT_CMD_BINARY_MODE		0
#define PIT_CMD_BCD_MODE		1

#define PIT_CLOCK_FREQ 1193182

#define PIT_DEFAULT_STARTUP_COMMAND (PIT_CMD_CHANNEL_0 | PIT_CMD_LOBYTE_FIRST | \
	PIT_CMD_SQUAREWAVE | PIT_CMD_BINARY_MODE)

// tick counter / interrupt vector
unsigned int pit_tick_count = 0;
extern void __attribute__((cdecl)) pit_irq_handler (void);

// void i386_pit_initialize (): Initialize the programmable interval timer
// 8253-compatible chipset
void i386_pit_initialize () {

	// set int 0x20 to the PIT interrupt handler
	cpu_setvector (0x20, &pit_irq_handler);
	i386_pit_start (100);
}

// void i386_pit_start (): Change the timer frequency and start a new timer
// at freq Hz. timer trips IRQ0 when fired
void i386_pit_start (unsigned int freq) {

	// calculate the frequency value
	unsigned short reload_value = PIT_CLOCK_FREQ / freq;

	// write the command, then the values to the system
	outportb (PIT_COMMAND_PORT, PIT_DEFAULT_STARTUP_COMMAND);
	outportb (PIT_CHANNEL0_DATA_PORT, (unsigned char)(reload_value & 0xff00));
	outportb (PIT_CHANNEL0_DATA_PORT, (unsigned char)(reload_value >> 8));
}
