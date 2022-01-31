/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_PCR_H
#define SOC_INTEL_COMMON_BLOCK_PCR_H

/* Port Id lives in bits 23:16 and register offset lives in 15:0 of address. */
#define PCR_PORTID_SHIFT	16

#if !defined(__ACPI__)
#include <types.h>
#include <device/pci_ops.h>

uint32_t pcr_read32(uint8_t pid, uint16_t offset);
uint16_t pcr_read16(uint8_t pid, uint16_t offset);
uint8_t pcr_read8(uint8_t pid, uint16_t offset);
void pcr_write32(uint8_t pid, uint16_t offset, uint32_t indata);
void pcr_write16(uint8_t pid, uint16_t offset, uint16_t indata);
void pcr_write8(uint8_t pid, uint16_t offset, uint8_t indata);
void pcr_rmw32(uint8_t pid, uint16_t offset, uint32_t anddata,
		uint32_t ordata);
void pcr_rmw16(uint8_t pid, uint16_t offset, uint16_t anddata,
		uint16_t ordata);
void pcr_rmw8(uint8_t pid, uint16_t offset, uint8_t anddata,
		uint8_t ordata);
void pcr_or32(uint8_t pid, uint16_t offset, uint32_t ordata);
void pcr_or16(uint8_t pid, uint16_t offset, uint16_t ordata);
void pcr_or8(uint8_t pid, uint16_t offset, uint8_t ordata);

/* SBI command */
enum {
	MEM_READ = 0x00,
	MEM_WRITE = 0x01,
	PCI_CONFIG_READ = 0x04,
	PCI_CONFIG_WRITE = 0x05,
	PCR_READ = 0x06,
	PCR_WRITE = 0x07,
	GPIO_LOCK_UNLOCK = 0x13,
};

struct pcr_sbi_msg {
	uint8_t pid; /* 0x00 - Port ID of the SBI message */
	uint32_t offset; /* 0x01 - Register offset of the SBI message */
	uint8_t opcode; /* 0x05 - Opcode */
	bool is_posted; /* 0x06 - Posted message */
	uint16_t fast_byte_enable; /* 0x07 - First Byte Enable */
	uint16_t bar; /* 0x09 - base address */
	uint16_t fid; /* 0x0B - Function ID */
};

/*
 * API to perform sideband communication
 *
 * Input:
 * struct pcr_sbi_msg
 * data - read/write for sbi message
 * response -
 * 0 - successful
 * 1 - unsuccessful
 * 2 - powered down
 * 3 - multi-cast mixed
 *
 * Output:
 * 0: SBI message is successfully completed
 * -1: SBI message failure
 */
int pcr_execute_sideband_msg(pci_devfn_t dev, struct pcr_sbi_msg *msg, uint32_t *data,
		uint8_t *response);

/* Get the starting address of the port's registers. */
void *pcr_reg_address(uint8_t pid, uint16_t offset);
#endif /* if !defined(__ACPI__) */

#endif	/* SOC_INTEL_COMMON_BLOCK_PCR_H */
