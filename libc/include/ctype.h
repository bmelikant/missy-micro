#ifndef _MISSY_LIBC_CTYPE_H
#define _MISSY_LIBC_CTYPE_H 1

#ifdef _cplusplus
extern "C" {
#endif

extern char _ctype[];

/* Constant definitions */

#define CT_UP 	0x01
#define CT_LOW	0x02
#define CT_DIG	0x04
#define CT_CTL	0x08
#define CT_PUN	0x10
#define CT_WHT	0x20
#define CT_HEX	0x40
#define CT_SP	0x80

/* ctype macros */

#define iscntrl(c)	((_ctype+1)[(unsigned)c] & (CT_CTL))

int isalpha(int c);
int isdigit(int c);
int isalnum(int c);
int isspace(int c);
int isupper(int c);

#ifdef _cplusplus
}
#endif

#endif
