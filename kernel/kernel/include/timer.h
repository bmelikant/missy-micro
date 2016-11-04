#ifndef _MISSY_KERNEL_TIMER_H
#define _MISSY_KERNEL_TIMER_H

#ifdef _cplusplus
extern "C" {
#endif

#include <stddef.h>

/* timer interface routines */
int 			timer_initialize 	();
int				timer_start 		(int freq);

#ifdef _cplusplus
}
#endif

#endif /* _MISSY_KERNEL_TIMER_H */
