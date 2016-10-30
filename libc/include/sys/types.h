#ifndef _MISSY_LIBC_TYPES_H
#define _MISSY_LIBC_TYPES_H 1

#ifdef _cplusplus
extern "C" {
#endif

#if __BITS_PER_LONG != 64
typedef int		ssize_t;
#else
typedef long	ssize_t;
#endif

#ifdef _cplusplus
}
#endif

#endif
