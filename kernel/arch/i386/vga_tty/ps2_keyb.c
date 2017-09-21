/*
 * ps2_kb.c
 *
 *  Created on: Aug 27, 2017
 *      Author: bmelikant
 */

// kernel private includes
#include <include/cpu.h>
#include <kernel/tty.h>

// architecture includes
#include <i386/include/ps2_kb.h>
#include <i386/include/pic.h>

#include <stdint.h>
#include <stddef.h>
#include <ctype.h>

#define KB_QUEUE_SIZE 256

typedef struct KEYMAP_ENTRY {

	unsigned int lowercase;
	unsigned int uppercase;

} keymap_entry;

// scanset map
keymap_entry keymap[] = {

	{ KEY_UNKNOWN, KEY_UNKNOWN },		//0
	{ KEY_ESCAPE, KEY_ESCAPE },			//1
	{ KEY_1, KEY_EXCLAMATION },			//2
	{ KEY_2, KEY_AT },					//3
	{ KEY_3, KEY_HASH },				//4
	{ KEY_4, KEY_DOLLAR },				//5
	{ KEY_5, KEY_PERCENT },				//6
	{ KEY_6, KEY_CARRET },				//7
	{ KEY_7, KEY_AMPERSAND },			//8
	{ KEY_8, KEY_ASTERISK },			//9
	{ KEY_9, KEY_LEFTPARENTHESIS },		//0xa
	{ KEY_0, KEY_RIGHTPARENTHESIS },	//0xb
	{ KEY_MINUS, KEY_UNDERSCORE },		//0xc
	{ KEY_EQUAL, KEY_PLUS },			//0xd
	{ KEY_BACKSPACE, KEY_BACKSPACE },	//0xe
	{ KEY_TAB, KEY_TAB },				//0xf
	{ KEY_Q, KEY_UPPER_Q },			//0x10
	{ KEY_W, KEY_UPPER_W },			//0x11
	{ KEY_E, KEY_UPPER_E },			//0x12
	{ KEY_R, KEY_UPPER_R },			//0x13
	{ KEY_T, KEY_UPPER_T },			//0x14
	{ KEY_Y, KEY_UPPER_Y },			//0x15
	{ KEY_U, KEY_UPPER_U },			//0x16
	{ KEY_I, KEY_UPPER_I },			//0x17
	{ KEY_O, KEY_UPPER_O },			//0x18
	{ KEY_P, KEY_UPPER_P },			//0x19
	{ KEY_LEFTBRACKET, KEY_LEFTCURL },		//0x1a
	{ KEY_RIGHTBRACKET, KEY_RIGHTCURL }, 	//0x1b
	{ KEY_RETURN, KEY_RETURN },			//0x1c
	{ KEY_LCTRL, KEY_LCTRL },			//0x1d
	{ KEY_A, KEY_UPPER_A },			//0x1e
	{ KEY_S, KEY_UPPER_S },			//0x1f
	{ KEY_D, KEY_UPPER_D },			//0x20
	{ KEY_F, KEY_UPPER_F },			//0x21
	{ KEY_G, KEY_UPPER_G },			//0x22
	{ KEY_H, KEY_UPPER_H },			//0x23
	{ KEY_J, KEY_UPPER_J },			//0x24
	{ KEY_K, KEY_UPPER_K },			//0x25
	{ KEY_L, KEY_UPPER_L },			//0x26
	{ KEY_SEMICOLON, KEY_COLON },		//0x27
	{ KEY_QUOTE, KEY_QUOTEDOUBLE },		//0x28
	{ KEY_GRAVE, KEY_TILDE },			//0x29
	{ KEY_LSHIFT, KEY_LSHIFT },			//0x2a
	{ KEY_BACKSLASH, KEY_BAR },			//0x2b
	{ KEY_Z, KEY_UPPER_Z },			//0x2c
	{ KEY_X, KEY_UPPER_X },			//0x2d
	{ KEY_C, KEY_UPPER_C },			//0x2e
	{ KEY_V, KEY_UPPER_V },			//0x2f
	{ KEY_B, KEY_UPPER_B },			//0x30
	{ KEY_N, KEY_UPPER_N },			//0x31
	{ KEY_M, KEY_UPPER_M },			//0x32
	{ KEY_COMMA, KEY_LESS },			//0x33
	{ KEY_DOT, KEY_GREATER },			//0x34
	{ KEY_SLASH, KEY_QUESTION },		//0x35
	{ KEY_RSHIFT, KEY_RSHIFT },			//0x36
	{ KEY_KP_ASTERISK, KEY_KP_ASTERISK },	//0x37
	{ KEY_RALT,	KEY_RALT },					//0x38
	{ KEY_SPACE, KEY_SPACE },				//0x39
	{ KEY_CAPSLOCK,	KEY_CAPSLOCK },			//0x3a
	{ KEY_F1, KEY_F1 },						//0x3b
	{ KEY_F2, KEY_F2 },						//0x3c
	{ KEY_F3, KEY_F3 },						//0x3d
	{ KEY_F4, KEY_F4 },						//0x3e
	{ KEY_F5, KEY_F5 },						//0x3f
	{ KEY_F6, KEY_F6 },						//0x40
	{ KEY_F7, KEY_F7 },						//0x41
	{ KEY_F8, KEY_F8 },						//0x42
	{ KEY_F9, KEY_F9 },						//0x43
	{ KEY_F10, KEY_F10 },					//0x44
	{ KEY_KP_NUMLOCK, KEY_KP_NUMLOCK },		//0x45
	{ KEY_SCROLLLOCK, KEY_SCROLLLOCK },		//0x46
	{ KEY_HOME, KEY_HOME },					//0x47
	{ KEY_KP_8,	KEY_KP_8 },					//0x48	// keypad up arrow
	{ KEY_PAGEUP, KEY_PAGEUP },				//0x49
	{ KEY_KP_2, KEY_KP_2 },					//0x50	// keypad down arrow
	{ KEY_KP_3, KEY_KP_3 },					//0x51	// keypad page down
	{ KEY_KP_0,	KEY_KP_0 },					//0x52	// keypad insert key
	{ KEY_KP_DECIMAL, KEY_KP_DECIMAL },		//0x53	//keypad delete key
	{ KEY_UNKNOWN, KEY_UNKNOWN },			//0x54
	{ KEY_UNKNOWN, KEY_UNKNOWN },			//0x55
	{ KEY_UNKNOWN, KEY_UNKNOWN },			//0x56
	{ KEY_F11, KEY_F11 },					//0x57
	{ KEY_F12, KEY_F12 }					//0x58
};

void ps2_controller_wait_inp ();
void ps2_controller_wait_outp ();
void ps2_keyboard_enqueue (uint32_t scancode);
int  ps2_keyboard_dequeue ();

extern void __attribute__((cdecl)) keyb_interrupt ();

// the currently pressed (or released) key
int key_queue[KB_QUEUE_SIZE];
size_t queue_tail = 0;

bool _shift = false;

// initialize the keyboard!!
unsigned long ps2_keyb_initialize () {

	// usb should be initialized; see if the controllers are enabled (TODO, need ACPI support first)

	/*
	// disable devices
	outportb (PS2_CONTROLLER_COMMAND, PS2_CONTROLLER_DISABLE_FIRST);
	outportb (PS2_CONTROLLER_COMMAND, PS2_CONTROLLER_DISABLE_SECOND);

	terminal_puts ("Devices disabled");

	// flush the output buffer
	while ((inportb (PS2_CONTROLLER_STATUS) & PS2_OUTBUFFER_STATUS))
		inportb (PS2_KEYBOARD_PORT);

	terminal_puts ("Cleared buffer");

	// read the configuration byte
	outportb (PS2_CONTROLLER_COMMAND, PS2_CONTROLLER_READ_CONFIG);
	uint8_t config_byte = inportb (PS2_CONTROLLER_STATUS);

	terminal_puts ("Read configuration byte");

	// disable interrupts and translation
	config_byte &= ~(PS2_PORT1_INTERRUPT | PS2_PORT2_INTERRUPT | PS2_TRANSLATION_ENABLE);

	outportb (PS2_CONTROLLER_COMMAND, PS2_CONTROLLER_WRITE_CONFIG);
	ps2_controller_wait_inp ();
	outportb (PS2_CONTROLLER_DATA, config_byte);

	terminal_puts ("Disabled interrupts and translation");

	// controller self-test
	outportb (PS2_CONTROLLER_COMMAND, PS2_CONTROLLER_SELFTEST);
	ps2_controller_wait_outp ();
	uint8_t response = inportb (PS2_CONTROLLER_DATA);

	terminal_printf ("Got 0x%x back from the controller on self-test\n", response);

	if (inportb (PS2_CONTROLLER_DATA) != 0x55)
		return -1;

	terminal_puts ("Controller passed self-test");

	// test first interface
	outportb (PS2_CONTROLLER_COMMAND, PS2_CONTROLLER_TEST_FIRST);
	ps2_controller_wait_outp ();

	if (inportb (PS2_CONTROLLER_DATA))
		return -1;

	// enable device on port 1
	outportb (PS2_CONTROLLER_COMMAND, PS2_CONTROLLER_ENABLE_FIRST);

	// reset device on port 1
	outportb (PS2_KEYBOARD_PORT, PS2_KEYBOARD_RESET);
	ps2_controller_wait_outp ();

	if (inportb (PS2_KEYBOARD_PORT) != PS2_KEYBOARD_OKAY)
		return -1;

	*/

	// insert the interrupt handler
	cpu_setvector (0x21, &keyb_interrupt);
	return 0;
}

// grab the next scancode
int ps2_keyb_getbyte () {

	int retchar = -1;

	if (queue_tail > 0)
		retchar = ps2_keyboard_dequeue ();

	return retchar;
}

// insert a scancode into the queue
void ps2_keyboard_enqueue (uint32_t scancode) {

	if (keymap[scancode &~ 0x80].lowercase == KEY_LSHIFT || keymap[scancode &~ 0x80].lowercase == KEY_RSHIFT) {
		
		if (scancode & 0x80)
			_shift = false;
		else
			_shift = true;
	}

	// if the scancode is a break code, queue the character
	else if (scancode & 0x80) {

		if (keymap[scancode &~ 0x80].lowercase == KEY_LSHIFT || keymap[scancode &~ 0x80].lowercase == KEY_RSHIFT)
			_shift = false;

		// push the keypress onto the tail of the queue
		if (queue_tail < KB_QUEUE_SIZE) {

			if (_shift)
				key_queue[queue_tail++] = keymap[scancode &~ 0x80].uppercase;
			else
				key_queue[queue_tail++] = keymap[scancode &~ 0x80].lowercase;
		}
	}
}

// remove a character from the queue and return it
int ps2_keyboard_dequeue () {

	// oops. return the character!
	int retval = key_queue[0];

	// copy the data forward
	for (size_t i = 0; i < queue_tail; i++)
		key_queue[i] = key_queue[i+1];

	if (queue_tail > 0)
		queue_tail--;

	return retval;
}

// wait for the controller to have data available
void ps2_controller_wait_outp () {

	while (!(inportb (PS2_CONTROLLER_STATUS) & PS2_OUTBUFFER_STATUS))
		;
}

// wait for the controller to accept the next byte
void ps2_controller_wait_inp () {

	while (inportb (PS2_CONTROLLER_STATUS) & PS2_INBUFFER_STATUS)
		terminal_puts ("looping inp...");
}
