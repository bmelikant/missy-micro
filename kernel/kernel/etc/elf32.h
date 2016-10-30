/*
 * elf32.h
 *
 *  Created on: Aug 21, 2016
 *      Author: bmelikant
 */

#ifndef MISSY_KERNEL_ELF32_H_
#define MISSY_KERNEL_ELF32_H_

#include <stdint.h>
#include <stddef.h>

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

struct elf32_hdr {

	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off  sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
};

#endif /* MISSY_KERNEL_ELF32_H_ */
