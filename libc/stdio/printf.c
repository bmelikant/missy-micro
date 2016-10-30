/*
 * printf.c: Implement standard C library printf routine (MISSY Microsystem Alpha 3.0)
 *
 *	Created 8/14/16 Ben Melikant
 *	Last edit: 8/14/16
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>

int printf (const char *str, ...) {

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
					putchar (num_str[i]);
			}

			// a binary format number?

			else if (*str == 'b') {

				unsigned int argument = va_arg (args, unsigned int);
				char num_str[36];

				itoa_s (num_str, argument, 2);

				for (size_t i = 0; i < strlen (num_str); i++)
					putchar (num_str[i]);
			}

			// a hexadecimal number?

			else if (*str == 'X' || *str == 'x') {

				unsigned int argument = va_arg (args, unsigned int);
				char num_str[20];

				itoa_s (num_str, argument, 16);

				for (size_t i = 0; i < strlen (num_str); i++)
					putchar (num_str[i]);
			}

			// a string?

			else if (*str == 's') {

				char *argument = va_arg (args, char*);

				while (*argument)
					putchar (*argument++);
			}

			// a character?

			else if (*str == 'c') {

				unsigned char argument = (unsigned char) va_arg (args, unsigned int);
				putchar (argument);
			}

			else
				putchar (*str);

			// increment the pointer
			str++;
		}

		else
			putchar (*str++);		// just print out the character
	}

	return 1;
}
