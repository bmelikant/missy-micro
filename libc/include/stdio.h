#ifndef _MISSY_LIBC_STDIO_H
#define _MISSY_LIBC_STDIO_H 1

#ifdef _cplusplus
extern "C" {
#endif

/* Standard I/O library functions */

// EOF declaration
#define EOF -1

// output functions
int printf 	(const char *fmt, ...);
int putchar 	(int c);
int puts	(const char *str);

#ifdef _cplusplus
}
#endif

#endif