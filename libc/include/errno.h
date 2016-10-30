#ifndef _MISSY_LIBC_ERRNO_H
#define _MISSY_LIBC_ERRNO_H 1

#ifdef _cplusplus
extern "C" {
#endif

/*
 * errno.h: C standard error library for Missy OS v2.0b
 * Ben Melikant, 1/31/2016
 * Last update: 8/14/16 - brought into MISSY Microsystem Alpha 3.0 build as libc/include/errno.h
 */

extern int errno;

#define EDOM		0x00000001
#define EILSEQ		0x00000002
#define ERANGE		0x00000003
#define ENOMEM		0x00000004

#ifdef _cplusplus
}
#endif

#endif
