/*
 * ps2_kb.h
 *
 *  Created on: Aug 27, 2017
 *      Author: bmelikant
 */

#ifndef KERNEL_ARCH_I386_INCLUDE_PS2_KB_H_
#define KERNEL_ARCH_I386_INCLUDE_PS2_KB_H_

/* Keyboard I/O Ports */
#define PS2_KEYBOARD_PORT		0x60
#define PS2_CONTROLLER_DATA		0x60
#define PS2_CONTROLLER_STATUS	0x64
#define PS2_CONTROLLER_COMMAND	0x64

/* Keyboard command bytes */
#define PS2_SET_LEDS			0xed
#define PS2_ECHO_COMMAND		0xee
#define PS2_GET_SCANSET			0xf0
#define PS2_IDENTIFY_KEYBOARD	0xf2
#define PS2_SET_TYPEMATIC_RPT	0xf3
#define PS2_KEYBOARD_ENABLE		0xf4
#define PS2_KEYBOARD_DISABLE	0xf5
#define PS2_KEYBOARD_RESET		0xff

/* Keyboard data */
#define PS2_KEYBOARD_SCRLOCK		0x01
#define PS2_KEYBOARD_NUMLOCK		0x02
#define PS2_KEYBOARD_CAPSLOCK		0x04
#define PS2_KEYBOARD_OKAY			0xaa
#define PS2_KEYBOARD_MAX_RETRIES	3

/* PS/2 Controller Status bits */
#define PS2_OUTBUFFER_STATUS		0x01
#define PS2_INBUFFER_STATUS			0x02
#define PS2_SYSTEM_FLAG				0x04
#define PS2_COMMAND_OR_DATA			0x08
#define PS2_TIMEOUT_ERROR			0x40
#define PS2_PARITY_ERROR			0x80

/* PS/2 controller configuration bits */
#define PS2_PORT1_INTERRUPT			0x01
#define PS2_PORT2_INTERRUPT			0x02
#define PS2_TRANSLATION_ENABLE		0x40

/* PS/2 controller commands */
#define PS2_CONTROLLER_DISABLE_FIRST	0xad
#define PS2_CONTROLLER_DISABLE_SECOND	0xa7
#define PS2_CONTROLLER_ENABLE_FIRST		0xae
#define PS2_CONTROLLER_ENABLE_SECOND	0xa8
#define PS2_CONTROLLER_READ_CONFIG		0x20
#define PS2_CONTROLLER_WRITE_CONFIG		0x60
#define PS2_CONTROLLER_SELFTEST			0xaa
#define PS2_CONTROLLER_TEST_FIRST		0xab

enum KEYCODE {

// Alphanumeric keys ////////////////

	KEY_SPACE             = ' ',
	KEY_0                 = '0',
	KEY_1                 = '1',
	KEY_2                 = '2',
	KEY_3                 = '3',
	KEY_4                 = '4',
	KEY_5                 = '5',
	KEY_6                 = '6',
	KEY_7                 = '7',
	KEY_8                 = '8',
	KEY_9                 = '9',

	KEY_A                 = 'a',
	KEY_B                 = 'b',
	KEY_C                 = 'c',
	KEY_D                 = 'd',
	KEY_E                 = 'e',
	KEY_F                 = 'f',
	KEY_G                 = 'g',
	KEY_H                 = 'h',
	KEY_I                 = 'i',
	KEY_J                 = 'j',
	KEY_K                 = 'k',
	KEY_L                 = 'l',
	KEY_M                 = 'm',
	KEY_N                 = 'n',
	KEY_O                 = 'o',
	KEY_P                 = 'p',
	KEY_Q                 = 'q',
	KEY_R                 = 'r',
	KEY_S                 = 's',
	KEY_T                 = 't',
	KEY_U                 = 'u',
	KEY_V                 = 'v',
	KEY_W                 = 'w',
	KEY_X                 = 'x',
	KEY_Y                 = 'y',
	KEY_Z                 = 'z',

	KEY_UPPER_A  = 'A',
	KEY_UPPER_B  = 'B',
	KEY_UPPER_C  = 'C',
	KEY_UPPER_D  = 'D',
	KEY_UPPER_E  = 'E',
	KEY_UPPER_F  = 'F',
	KEY_UPPER_G  = 'G',
	KEY_UPPER_H  = 'H',
	KEY_UPPER_I  = 'I',
	KEY_UPPER_J  = 'J',
	KEY_UPPER_K  = 'K',
	KEY_UPPER_L  = 'L',
	KEY_UPPER_M  = 'M',
	KEY_UPPER_N  = 'N',
	KEY_UPPER_O  = 'O',
	KEY_UPPER_P  = 'P',
	KEY_UPPER_Q  = 'Q',
	KEY_UPPER_R  = 'R',
	KEY_UPPER_S  = 'S',
	KEY_UPPER_T  = 'T',
	KEY_UPPER_U  = 'U',
	KEY_UPPER_V  = 'V',
	KEY_UPPER_W  = 'W',
	KEY_UPPER_X  = 'X',
	KEY_UPPER_Y  = 'Y',
	KEY_UPPER_Z  = 'Z',

	KEY_RETURN            = '\r',
	KEY_ESCAPE            = 0x1001,
	KEY_BACKSPACE         = '\b',

// Arrow keys ////////////////////////

	KEY_UP                = 0x1100,
	KEY_DOWN              = 0x1101,
	KEY_LEFT              = 0x1102,
	KEY_RIGHT             = 0x1103,

// Function keys /////////////////////

	KEY_F1                = 0x1201,
	KEY_F2                = 0x1202,
	KEY_F3                = 0x1203,
	KEY_F4                = 0x1204,
	KEY_F5                = 0x1205,
	KEY_F6                = 0x1206,
	KEY_F7                = 0x1207,
	KEY_F8                = 0x1208,
	KEY_F9                = 0x1209,
	KEY_F10               = 0x120a,
	KEY_F11               = 0x120b,
	KEY_F12               = 0x120b,
	KEY_F13               = 0x120c,
	KEY_F14               = 0x120d,
	KEY_F15               = 0x120e,

	KEY_DOT               = '.',
	KEY_COMMA             = ',',
	KEY_COLON             = ':',
	KEY_SEMICOLON         = ';',
	KEY_SLASH             = '/',
	KEY_BACKSLASH         = '\\',
	KEY_PLUS              = '+',
	KEY_MINUS             = '-',
	KEY_ASTERISK          = '*',
	KEY_EXCLAMATION       = '!',
	KEY_QUESTION          = '?',
	KEY_QUOTEDOUBLE       = '\"',
	KEY_QUOTE             = '\'',
	KEY_EQUAL             = '=',
	KEY_HASH              = '#',
	KEY_PERCENT           = '%',
	KEY_AMPERSAND         = '&',
	KEY_UNDERSCORE        = '_',
	KEY_LEFTPARENTHESIS   = '(',
	KEY_RIGHTPARENTHESIS  = ')',
	KEY_LEFTBRACKET       = '[',
	KEY_RIGHTBRACKET      = ']',
	KEY_LEFTCURL          = '{',
	KEY_RIGHTCURL         = '}',
	KEY_DOLLAR            = '$',
	KEY_POUND             = '£',
	KEY_EURO              = '$',
	KEY_LESS              = '<',
	KEY_GREATER           = '>',
	KEY_BAR               = '|',
	KEY_GRAVE             = '`',
	KEY_TILDE             = '~',
	KEY_AT                = '@',
	KEY_CARRET            = '^',

// Numeric keypad //////////////////////

	KEY_KP_0              = '0',
	KEY_KP_1              = '1',
	KEY_KP_2              = '2',
	KEY_KP_3              = '3',
	KEY_KP_4              = '4',
	KEY_KP_5              = '5',
	KEY_KP_6              = '6',
	KEY_KP_7              = '7',
	KEY_KP_8              = '8',
	KEY_KP_9              = '9',
	KEY_KP_PLUS           = '+',
	KEY_KP_MINUS          = '-',
	KEY_KP_DECIMAL        = '.',
	KEY_KP_DIVIDE         = '/',
	KEY_KP_ASTERISK       = '*',
	KEY_KP_NUMLOCK        = 0x300f,
	KEY_KP_ENTER          = 0x3010,

	KEY_TAB               = 0x4000,
	KEY_CAPSLOCK          = 0x4001,

// Modify keys ////////////////////////////

	KEY_LSHIFT            = 0x4002,
	KEY_LCTRL             = 0x4003,
	KEY_LALT              = 0x4004,
	KEY_LWIN              = 0x4005,
	KEY_RSHIFT            = 0x4006,
	KEY_RCTRL             = 0x4007,
	KEY_RALT              = 0x4008,
	KEY_RWIN              = 0x4009,

	KEY_INSERT            = 0x400a,
	KEY_DELETE            = 0x400b,
	KEY_HOME              = 0x400c,
	KEY_END               = 0x400d,
	KEY_PAGEUP            = 0x400e,
	KEY_PAGEDOWN          = 0x400f,
	KEY_SCROLLLOCK        = 0x4010,
	KEY_PAUSE             = 0x4011,

	KEY_UNKNOWN,
	KEY_NUMKEYCODES
};

unsigned long ps2_keyb_initialize 	();			// initialize the keyboard minidriver
int           ps2_keyb_getbyte 		();			// get the next keyboard scancode

#endif /* KERNEL_ARCH_I386_INCLUDE_PS2_KB_H_ */
