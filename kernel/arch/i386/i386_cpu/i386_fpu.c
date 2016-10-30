/*
 * i386_fpu.c: MISSY Microsystem initialization file for the i386 floating point unit
 * Initializes and ensures proper operation of FPU operations
 *
 *  Created on: Aug 15, 2016
 *      Author: Ben Melikant
 */

// standard includes
#include "i386_fpu.h"

#include <cpuid.h>

// fpu includes

// constant definitions
#define CPUID_EDX_L1_FPU_BIT	0x00000001
#define CPUID_EDX_L1_SSE_BIT	0x02000000

#define cpu_feature_check(x,y)	(x & y)

extern int i386_fpu_init ();

// i386_fpu_initialize () Initialize the floating point unit
// inputs: none, returns: 0 on success, nonzero on error
int i386_fpu_initialize () {

	// make sure we can call CPUID levels needed
	unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;

	// try to call CPUID. If not supported, exit with error
	if (__get_cpuid (0x01, &eax, &ebx, &ecx, &edx) == 0)
		return I386_FPU_CPUID_BAD_LVL;

	// now check the feature set for the FPU bit
	if (!cpu_feature_check(CPUID_EDX_L1_FPU_BIT, edx))
		return I386_FPU_NOT_FOUND;

	// enable with SSE if available on the CPU
	if (cpu_feature_check(CPUID_EDX_L1_SSE_BIT, edx))
		i386_fpu_init (0x01);
	else
		i386_fpu_init (0x00);

	return I386_FPU_SUCCESS;
}
