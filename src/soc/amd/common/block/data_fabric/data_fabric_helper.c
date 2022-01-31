/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/data_fabric.h>
#include <amdblocks/pci_devs.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <soc/data_fabric.h>
#include <soc/pci_devs.h>
#include <types.h>
#include "data_fabric_def.h"

static void data_fabric_set_indirect_address(uint8_t func, uint16_t reg, uint8_t instance_id)
{
	uint32_t fabric_indirect_access_reg = DF_IND_CFG_INST_ACC_EN;
	/* Register offset field [10:2] in this register corresponds to [10:2] of the
	   requested offset. */
	fabric_indirect_access_reg |= reg & DF_IND_CFG_ACC_REG_MASK;
	fabric_indirect_access_reg |=
		(func << DF_IND_CFG_ACC_FUN_SHIFT) & DF_IND_CFG_ACC_FUN_MASK;
	fabric_indirect_access_reg |= instance_id << DF_IND_CFG_INST_ID_SHIFT;
	pci_write_config32(SOC_DF_F4_DEV, DF_FICAA_BIOS, fabric_indirect_access_reg);
}

uint32_t data_fabric_read32(uint8_t function, uint16_t reg, uint8_t instance_id)
{
	/* Broadcast reads might return unexpected results when a register has different
	   contents in the different instances. */
	if (instance_id == BROADCAST_FABRIC_ID)
		return data_fabric_broadcast_read32(function, reg);

	/* non-broadcast data fabric accesses need to be done via indirect access */
	data_fabric_set_indirect_address(function, reg, instance_id);
	return pci_read_config32(SOC_DF_F4_DEV, DF_FICAD_LO);
}

void data_fabric_write32(uint8_t function, uint16_t reg, uint8_t instance_id, uint32_t data)
{
	if (instance_id == BROADCAST_FABRIC_ID) {
		data_fabric_broadcast_write32(function, reg, data);
		return;
	}

	/* non-broadcast data fabric accesses need to be done via indirect access */
	data_fabric_set_indirect_address(function, reg, instance_id);
	pci_write_config32(SOC_DF_F4_DEV, DF_FICAD_LO, data);
}

void data_fabric_print_mmio_conf(void)
{
	printk(BIOS_SPEW,
		"=== Data Fabric MMIO configuration registers ===\n"
		"Addresses are shifted to the right by 16 bits.\n"
		"idx  control     base    limit\n");
	for (unsigned int i = 0; i < NUM_NB_MMIO_REGS; i++) {
		printk(BIOS_SPEW, " %2u %8x %8x %8x\n",
			i,
			data_fabric_broadcast_read32(0, NB_MMIO_CONTROL(i)),
			data_fabric_broadcast_read32(0, NB_MMIO_BASE(i)),
			data_fabric_broadcast_read32(0, NB_MMIO_LIMIT(i)));
	}
}

void data_fabric_disable_mmio_reg(unsigned int reg)
{
	data_fabric_broadcast_write32(0, NB_MMIO_CONTROL(reg),
		IOMS0_FABRIC_ID << DF_MMIO_DST_FABRIC_ID_SHIFT);
	data_fabric_broadcast_write32(0, NB_MMIO_BASE(reg), 0);
	data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(reg), 0);
}

static bool is_mmio_reg_disabled(unsigned int reg)
{
	uint32_t val = data_fabric_broadcast_read32(0, NB_MMIO_CONTROL(reg));
	return !(val & (DF_MMIO_WE | DF_MMIO_RE));
}

int data_fabric_find_unused_mmio_reg(void)
{
	for (unsigned int i = 0; i < NUM_NB_MMIO_REGS; i++) {
		if (is_mmio_reg_disabled(i))
			return i;
	}
	return -1;
}
