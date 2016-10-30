/*
 * tty.c: Basic VGA terminal for kernel boot process - MISSY Microsystem Alpha 3.0
 *
 *	Created 8/14/16 Ben Melikant
 *	Last edit: 8/14/16
 */

#include <kernel/tty.h>

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>


// constant definitions
#define VGA_VIDEO_ROWS		25		// max rows
#define VGA_VIDEO_COLUMNS	80		// max columns
#define VGA_VIDEO_MEMORY	0xb8000		// video memory start
#define VGA_VIDEO_DEFCOLOR	0x0f		// screen is white-on-black by default
#define VGA_TABSIZE			4		// tab size for our driver

static int _curX, _curY;
static uint8_t color_attrib;
static uint16_t *video_memory;

// int terminal_initialize (): Initialize the VGA display
int terminal_initialize () {

	_curX = _curY = 0;
	color_attrib = VGA_VIDEO_DEFCOLOR;
	video_memory = (uint16_t *) VGA_VIDEO_MEMORY;

	return VGA_VIDEO_ROWS*VGA_VIDEO_COLUMNS;
}

// int terminal_printchar (): Place a character on the vga terminal display
int terminal_printchar (int c) {

	// is our character a linefeed character?
	if (c == '\n') {

		_curX = 0;
		_curY++;

		if (_curY > VGA_VIDEO_ROWS-1) {

			terminal_scroll ();
			_curY--;
		}
	}

	// on tabs, write out four spaces
	else if (c == '\t') {

		for (size_t i = 0; i < VGA_TABSIZE; i++)
			terminal_printchar (0x20);
	}

	else {

		// get a pointer to the current write location of video memory
		uint16_t idx = (_curY*VGA_VIDEO_COLUMNS)+_curX;
		video_memory[idx] = (uint16_t) ((color_attrib << 8) | c);

		// increment the write location. Handle buffer overruns as well
		if (_curX++ >= VGA_VIDEO_COLUMNS) {

			_curX = 0;			// clear the x value and move to the next line
			_curY++;

			if (_curY > VGA_VIDEO_ROWS-1) {

				terminal_scroll ();
				_curY--;
			}
		}
	}

	return c;
}

// int terminal_puts (): Put a line of text on the VGA display (appends newline)
int terminal_puts (const char *str) {

	unsigned char *s = (unsigned char *) str;

	while (*s)
		terminal_printchar (*s++);

	terminal_printchar ('\n');

	return (int)((s - (unsigned char *)(str)) + 1);
}

void terminal_printf (const char *str, ...) {

	// use va_start to get the list of items
	va_list args;
	va_start (args, str);

	while (*str) {

		// if the '%' character appears, it is a formatting request

		if (*str == '%') {

			str++;		// increment the string pointer to get the format item

			// is an integer print requested?

			if (*str == 'd' || *str == 'i') {

				unsigned int argument = va_arg (args, unsigned int);
				char num_str[20];

				itoa_s (num_str, argument, 10);

				for (size_t i = 0; i < strlen (num_str); i++)
					terminal_printchar (num_str[i]);
			}

			// a binary format number?

			else if (*str == 'b') {

				unsigned int argument = va_arg (args, unsigned int);
				char num_str[36];

				itoa_s (num_str, argument, 2);

				for (size_t i = 0; i < strlen (num_str); i++)
					terminal_printchar (num_str[i]);
			}

			// a hexadecimal number?

			else if (*str == 'X' || *str == 'x') {

				unsigned int argument = va_arg (args, unsigned int);
				char num_str[20];

				itoa_s (num_str, argument, 16);

				for (size_t i = 0; i < strlen (num_str); i++)
					terminal_printchar (num_str[i]);
			}

			// a string?

			else if (*str == 's') {

				char *argument = va_arg (args, char*);

				while (*argument)
					terminal_printchar (*argument++);
			}

			// a character?

			else if (*str == 'c') {

				unsigned char argument = (unsigned char) va_arg (args, unsigned int);
				terminal_printchar (argument);
			}

			else
				terminal_printchar (*str);

			// increment the pointer
			str++;
		}

		else
			terminal_printchar (*str++);		// just print out the character
	}
}

// void terminal_scroll (): Scroll the display up one line
void terminal_scroll () {

	// copy each line upward
	for (int i = 0; i < VGA_VIDEO_COLUMNS*(VGA_VIDEO_ROWS-1); i++)
		video_memory[i] = video_memory[i+VGA_VIDEO_COLUMNS];

	// blank out the last line with the current color attribute
	for (int i = VGA_VIDEO_COLUMNS*(VGA_VIDEO_ROWS-1); i < VGA_VIDEO_COLUMNS*VGA_VIDEO_ROWS; i++)
		video_memory[i] = (uint16_t)((color_attrib << 8)|0x20);
}

// void terminal_clrscr (): Clear the terminal display
void terminal_clrscr () {

	// enter a loop to clear the video memory
	for (int i = 0; i < (VGA_VIDEO_ROWS*VGA_VIDEO_COLUMNS); i++)
		video_memory[i] = (uint16_t) ((color_attrib << 8) | 0x20);

	// clear the _curX and _curY variables
	_curX = _curY = 0;
}

// void terminal_makecolor (): Make a color value from two values
uint8_t terminal_makecolor (uint8_t fg, uint8_t bg) {

	return (uint8_t)(fg << 4 | (bg & 0xf0));
}

// void terminal_setcolor (): Set the color of the display
uint8_t terminal_setcolor (uint8_t color) {

	uint8_t tmp = color_attrib;
	color_attrib = color;
	return tmp;
}

// void terminal_setloc (): Set the print location on the display
void terminal_setloc (int x, int y) {

	_curX = x;
	_curY = y;
}
