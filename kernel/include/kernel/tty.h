#ifndef _MISSY_KERNEL_VGA_TTY_H
#define _MISSY_KERNEL_VGA_TTY_H

#ifdef _cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

int 		terminal_initialize 	();
int 		terminal_printchar  	(int c);
int			terminal_puts			(const char *str);
void		terminal_printf			(const char *s, ...);
void 		terminal_scroll 		();
void		terminal_clrscr			();
void		terminal_setloc			(int x, int y);
void        terminal_updatecursor   ();

unsigned long kernel_tty_initialize ();

ssize_t kernel_tty_read  (char *buf, size_t len);
ssize_t kernel_tty_write (char *buf, size_t len);

// keyboard driver event notification
void kernel_tty_notify_key ();

#ifdef _cplusplus
}
#endif

#endif
