/*
 * acpi.c
 *
 *  Created on: Aug 21, 2017
 *      Author: bmelikant
 */

// standard includes
#include <string.h>

// kernel public includes
#include <kernel/tty.h>

// kernel private includes
#include <include/acpi.h>
#include <include/kmemory.h>

#define EDBA_SEGMENT_LOC 0x40E
#define EDBA_MAX_SEEK	 1024

#define EDBA_BASE_ADDR 	0x000A0000
#define EDBA_END_ADDR  	0x000FFFFF
#define ACPI_ALIGN 		0x10

// table id strings
#define ACPI_SIGNATURE_RSDP 	"RSD PTR "
#define ACPI_SIGNATURE_RSDT		"RSDT"
#define ACPI_SIGNATURE_FADT		"FACP"
#define ACPI_SIGNATURE_DSDT		"DSDT"

// check through the EDBA to find the ACPI tables
RSDP_Descriptor *acpi_locate_rsdp () {

	// get a pointer to EDBA memory
	uint16_t *edba_segment_loc = (uint16_t *) EDBA_SEGMENT_LOC;
	uint16_t edba_segment = *edba_segment_loc;

	char *edba_area = (char *)(edba_segment*0x10);

	for (size_t i = 0; i < EDBA_MAX_SEEK; i += ACPI_ALIGN) {

		char *cmp_str = (char *)(edba_area+i);
		if (!strncmp (ACPI_SIGNATURE_RSDP, cmp_str, strlen(ACPI_SIGNATURE_RSDP))) {

			terminal_puts ("Found ACPI table in the EDBA");

			return (RSDP_Descriptor *) cmp_str;
		}
	}

	for (size_t i = 0; i < EDBA_END_ADDR-EDBA_BASE_ADDR; i += ACPI_ALIGN) {

		char *cmp_str = (char *)(EDBA_BASE_ADDR+i);

		if (!strncmp (ACPI_SIGNATURE_RSDP, cmp_str, strlen(ACPI_SIGNATURE_RSDP))) {

			terminal_puts ("Found ACPI table in the upper EDBA range");
			return (RSDP_Descriptor *)(cmp_str);
		}
	}

	return (RSDP_Descriptor *) NULL;
}

// compute the checksum of the RSDP descriptor to verify validity
unsigned char acpi_get_checksum (RSDP_Descriptor *rsdp) {

	unsigned int result = 0;
	unsigned char *acpi_byte = (unsigned char *) rsdp;

	for (size_t i = 0; i < sizeof (RSDP_Descriptor); i++)
		result += (unsigned int) *(acpi_byte+i);

	return (unsigned char) result;
}

// locate a table within the ACPI tables
void *locate_acpi_table (const char tablename[4], RSDP_Descriptor *rsdp) {

	// for ACPI revision 2, use the XDST. Otherwise, use rsdt
	if (rsdp->revision > 0) {

	}

	// grab the physical address of the rsdt, and align to 4KB for mapping
	else {

		void *rsdt_phys_block = (void *) (rsdp->rsdt_addr &~ 0xfff);
		void *rsdt_virt_block = kernel_request_temp_mapping(rsdt_phys_block, 1);

		// offset into the block
		ACPI_SDTHeader *rsdt = (ACPI_SDTHeader *)(rsdt_virt_block + (rsdp->rsdt_addr & 0xfff));
		terminal_printf ("Reading data from 0x%x, virtual block is 0x%x, physical is 0x%x\n",
				(unsigned int) rsdt, (unsigned int) rsdt_virt_block, (unsigned int) rsdt_phys_block);

		// sub-loop - check each header entry for the correct table
		while (((unsigned int) rsdt &~ 0xfff) == rsdt_virt_block) {

			//terminal_printf("Checking entry at 0x%x\n", (unsigned int) rsdt);
			//for (int i = 0; i < 4; i++)
				//terminal_printchar(rsdt->signature[i]);

			if (!strncmp(rsdt->signature, tablename, 4))
				return (void *)(rsdt_phys_block+((unsigned int) rsdt_virt_block & 0xfff));

			rsdt += rsdt->length;		// move to the next entry
		}
	}

	return NULL;
}