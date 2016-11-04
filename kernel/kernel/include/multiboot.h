#ifndef _MISSY_KERNEL_MULTIBOOT_H
#define _MISSY_KERNEL_MULTIBOOT_H

#ifdef _cplusplus
extern "C" {
#endif

// standard includes

#if !defined(_cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

// bit-test macros for the boot structure flags
#define bootflag_check(x,y) (x&y)

// bit definitions for boot flags

#define MEMSZ_PRESENT 			0x0001
#define BOOTDEV_PRESENT 		0x0002
#define CMDLINE_PRESENT 		0x0004
#define MODULES_PRESENT 		0x0008
#define AOUT_SYMTABLE_PRESENT	0x0010
#define ELF_SYMTABLE_PRESENT 	0x0020
#define MMAP_PRESENT    		0x0040
#define DRIVES_PRESENT  		0x0080
#define CONFIG_PRESENT      	0x0100
#define BOOTLDR_NAME_PRESENT 	0x0200
#define APM_TABLE_PRESENT		0x0400
#define VBE_TABLE_PRESENT		0x0800

// multiboot_info structure

struct multiboot_info {

	uint32_t flags;					// this field is required. Structure flags
	uint32_t memorySzLo;			// memory size (if flags[0] is set)
	uint32_t memorySzHi;
	uint32_t bootDevice;			// boot device (if flags[1] is set)
	uint32_t commandLine;			// kernel command line (if flags[2] is set)
	uint32_t mods_count;			// # of modules loaded (if flags[3] is set)
	uint32_t mods_addr;				// address of modules (if flags[3] is set)
	uint32_t elf_symbol_count;		// how many section symbols are present?
	uint32_t elf_entry_sz;			// how big is each symbol table entry?
	uint32_t elf_table_addr;		// where is the ELF symbol table located?
	uint32_t elf_string_table;		// strings representing ELF symbols
	uint32_t mmap_length;			// length of memory map (if flags[6] is set)
	uint32_t mmap_addr;				// address of memory map
	uint32_t drives_length;			// address & length of first drive structure
	uint32_t drives_addr;
	uint32_t config_table;			// ROM configuration table
	uint32_t bootldr_name;			// name of bootloader
	uint32_t apm_table;				// Advanced power management table
	uint32_t vbe_ctrl_info;			// information for video mode
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;

}__attribute__((packed));

// GRUB memory map structure info

struct memory_map_inf {

        uint32_t sz;            // size of this structure
        uint64_t baseAddr;      // base address of the memory region
        uint64_t length;        // memory section length
        uint32_t type;          // type of the region
 
} __attribute__((packed));

// GRUB boot modules structure

struct mboot_module_inf {

	unsigned int 	mod_addr_start;
	unsigned int 	mod_addr_end;
	const char 	*mod_string;

} __attribute__((packed));

#ifdef _cplusplus
}
#endif

#endif
