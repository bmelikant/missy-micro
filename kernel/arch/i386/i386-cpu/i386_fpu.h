/*
 * i386_fpu.h
 *
 *  Created on: Aug 15, 2016
 *      Author: bmelikant
 */

#ifndef MISSY_KERNEL_I386_FPU_H
#define MISSY_KERNEL_I386_FPU_H

#ifdef _cplusplus
extern "C" {
#endif

#define I386_FPU_SUCCESS			0x00
#define I386_FPU_CPUID_BAD_LVL	 	0x01
#define I386_FPU_NOT_FOUND			0x02

// initialize the floating point unit
int i386_fpu_initialize ();


#ifdef _cplusplus
}
#endif

#endif /* MISSY_KERNEL_I386_FPU_H */
