/*
 * acpi.h
 *
 *  Created on: Aug 21, 2017
 *      Author: bmelikant
 */

#ifndef _MISSY_KERNEL_ACPI_H_
#define _MISSY_KERNEL_ACPI_H_

#include <stdint.h>
#include <stddef.h>

#ifndef _cplusplus
#include <stdbool.h>
#endif

// Root system description pointer (all ACPI versions)
struct RSDP_DESCRIPTOR {

	char signature[8];
	uint8_t checksum;
	char oem_id[6];
	uint8_t revision;
	uint32_t rsdt_addr;

}__attribute__((packed));

// Root system description pointer (ACPI 2.0 and above)
struct RSDP_DESCRIPTOR_2 {

	struct RSDP_DESCRIPTOR rsdp;

	uint32_t len;
	uint64_t xsdt_addr;
	uint8_t checksum_ext;
	uint8_t reserved[3];
}__attribute__((packed));

// System Descriptor table header
struct ACPI_SDT_HEADER {

	char signature[4];				// tells with which table we are working
	uint32_t length;				// length of the table
	uint8_t revision;
	uint8_t checksum;				// checksum for the table. All bytes must sum to 0%0xff
	char oem_id[6];
	char oem_tbl_id[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;

}__attribute__((packed));

// generic address structure
struct GENERIC_ADDR {

	uint8_t addrspace;
	uint8_t bitwidth;
	uint8_t bitoffset;
	uint8_t access_sz;
	uint64_t address;

}__attribute__((packed));

// Fixed ACPI Description Table (FADT, signature FACP)
struct FIXED_ACPI_DESCRIPTION_TABLE {

	struct ACPI_SDT_HEADER acpi_header;
	uint32_t firmware_ctrl;
	uint32_t dsdt;

	uint8_t reserved;

	uint8_t ppm_profile;
	uint16_t sci_interrupt;
	uint32_t smi_cmdport;
	uint8_t acpi_enable;
	uint8_t acpi_disable;
	uint8_t s4bios_req;
	uint8_t pstate_ctrl;
	uint32_t pm1a_eventblock;
	uint32_t pm1b_eventblock;
	uint32_t pm1a_controlblock;
	uint32_t pm1b_controlblock;
	uint32_t pm2_controlblock;
	uint32_t pmtimer_block;
	uint32_t gpe0_block;
	uint32_t gpe1_block;
	uint8_t pm1_eventlength;
	uint8_t pm1_controllength;
	uint8_t pm2_controllength;
	uint8_t pm_timerlength;
	uint8_t gpe0_length;
	uint8_t gpe1_length;
	uint8_t gpe1_base;
	uint8_t cstate_control;
	uint16_t worst_c2_latency;
	uint16_t worst_c3_latency;
	uint16_t flush_size;
	uint16_t flush_stride;
	uint8_t duty_offset;
	uint8_t duty_width;
	uint8_t day_alarm;
	uint8_t month_alarm;
	uint8_t century;

	// only in ACPI 2.0+
	uint16_t bootarch_flags;
	uint8_t reserved2;
	uint32_t flags;

	struct GENERIC_ADDR reset_register;
	uint8_t reset_value;
	uint8_t reserved3[3];

	uint64_t x_firmwarectrl;
	uint64_t x_dsdt;

	struct GENERIC_ADDR x_pm1a_eventblock;
	struct GENERIC_ADDR x_pm1b_eventblock;
	struct GENERIC_ADDR x_pm1a_controlblock;
	struct GENERIC_ADDR x_pm1b_controlblock;
	struct GENERIC_ADDR x_pm2_controlblock;
	struct GENERIC_ADDR x_pmtimer_block;
	struct GENERIC_ADDR x_gpe0_block;
	struct GENERIC_ADDR x_gpe1_block;

}__attribute__((packed));

// Differentiated System Description Table (DSDT)
struct DIFF_SYS_DESCRIPTION_TABLE {

	char signature[4];		// should be "DSDT"
	uint32_t length;		// size of the entire table, including header and AML code
	uint8_t revision;
	uint8_t checksum;		// must sum to zero

	char oem_id[6];
	char oem_tableid[8];
	char oem_revision[4];

	char creator_id[4];
	char creator_revision[4];

	uint8_t aml_code[0];		// the AML that is contained within the DSDT

}__attribute__((packed));

// ACPI typedefs
typedef struct RSDP_DESCRIPTOR RSDP_Descriptor;
typedef struct RSDP_DESCRIPTOR_2 RSDP_Descriptor2;
typedef struct ACPI_SDT_HEADER ACPI_SDTHeader;
typedef struct FIXED_ACPI_DESCRIPTOR_TABLE ACPI_FADT;
typedef struct DIFF_SYS_DESCRIPTION_TABLE ACPI_DSDTHeader;

RSDP_Descriptor *acpi_locate_rsdp ();
unsigned char	 acpi_get_checksum  (RSDP_Descriptor *);


#endif /* _MISSY_KERNEL_ACPI_H_ */
