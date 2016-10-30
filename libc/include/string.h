#ifndef _MISSY_LIBC_STRING_H
#define _MISSY_LIBC_STRING_H 1

#include <stddef.h>

#ifdef _cplusplus
extern "C" {
#endif

/* Memory function prototypes */

void 	*memcpy 	(void *dst, const void *src, size_t count);
void 	*memmove 	(void *dst, const void *src, size_t count);
int	memcmp		(const void *s1, const void *s2, size_t count);
void	*memset		(void *blk, int val, size_t count);

// String function prototypes
size_t strlen (const char *s);

// conversion functions
char 	*itoa_s (char *str, unsigned int number, int base);
int	atoi 	(const char *numstr);
long	strtol	(const char *num, char **endptr, int base);

#ifdef _cplusplus
}
#endif

#endif