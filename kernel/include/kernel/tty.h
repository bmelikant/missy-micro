#ifndef _MISSY_KERNEL_VGA_TTY_H
#define _MISSY_KERNEL_VGA_TTY_H

#ifdef _cplusplus
extern "C" {
#endif

#include <stdint.h>

int 		terminal_initialize 	();
int 		terminal_printchar  	(int c);
int			terminal_puts		(const char *str);
void		terminal_printf		(const char *s, ...);
void 		terminal_scroll 	();
void		terminal_clrscr		();
void		terminal_setloc		(int x, int y);

#ifdef _cplusplus
}
#endif

#endif
