/*
 * cpu.h
 *
 *	Hardware-independent interface to the microprocessor. Allows startup of
 *  Created on: Aug 10, 2016
 *      Author: Ben Melikant
 */

#ifndef _MISSY_KERNEL_CPU_H
#define _MISSY_KERNEL_CPU_H

// standard headers
#ifndef _cplusplus
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

// architecture-specific includes / defines
#ifdef __arch_i386
#include <arch/i386/i386_cpu/i386_cpu.h>
#endif

#endif /* KERNEL_CPU_CPU_H_ */
