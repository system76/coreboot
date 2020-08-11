/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MATISSE_DATAFABRIC_H__
#define __SOC_MATISSE_DATAFABRIC_H__

#include <types.h>

/* D18F0 - Fabric Configuration registers */
#define IOMS0_FABRIC_ID			4

#define D18F0_VGAEN			0x80
#define   VGA_ADDR_ENABLE		BIT(0)

#define D18F0_PCI_MAP0	    0xA0
#define NB_PCI_MAP(reg)		((reg) * sizeof(uint32_t) + D18F0_PCI_MAP0)

#define D18F0_PIO_BASE0		0xC0
#define D18F0_PIO_LIMIT0	0xC4
#define NB_PIO_BASE(reg)		((reg) * 2 * sizeof(uint32_t) + D18F0_PIO_BASE0)
#define NB_PIO_LIMIT(reg)		((reg) * 2 * sizeof(uint32_t) + D18F0_PIO_LIMIT0)

#define D18F0_MMIO_BASE0		0x200
#define D18F0_MMIO_LIMIT0		0x204
#define   D18F0_MMIO_SHIFT		16
#define D18F0_MMIO_CTRL0		0x208
#define   MMIO_NP			BIT(16)
#define   MMIO_DST_FABRIC_ID_SHIFT	4
#define   MMIO_WE			BIT(1)
#define   MMIO_RE			BIT(0)
#define NUM_NB_MMIO_REGS		8
#define NB_MMIO_BASE(reg)		((reg) * 4 * sizeof(uint32_t) + D18F0_MMIO_BASE0)
#define NB_MMIO_LIMIT(reg)		((reg) * 4 * sizeof(uint32_t) + D18F0_MMIO_LIMIT0)
#define NB_MMIO_CONTROL(reg)		((reg) * 4 * sizeof(uint32_t) + D18F0_MMIO_CTRL0)

void data_fabric_init(void);
void data_fabric_set_mmio_np(void);

#endif /* __SOC_MATISSE_DATAFABRIC_H__ */
