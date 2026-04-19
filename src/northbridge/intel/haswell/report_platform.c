/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <device/pci_ops.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <types.h>

#include "haswell.h"

/* The PCI id name match comes from Intel document 472178 */
static struct {
	u16 dev_id;
	const char *dev_name;
} pch_table [] = {
	{0x8c41, "Mobile Engineering Sample"},
	{0x8c42, "Desktop Engineering Sample"},
	{0x8c44, "Z87"},
	{0x8c46, "Z85"},
	{0x8c49, "HM86"},
	{0x8c4a, "H87"},
	{0x8c4b, "HM87"},
	{0x8c4c, "Q85"},
	{0x8c4e, "Q87"},
	{0x8c4f, "QM87"},
	{0x8c50, "B85"},
	{0x8c52, "C222"},
	{0x8c54, "C224"},
	{0x8c56, "C226"},
	{0x8c5c, "H81"},
	{0x8cc1, "Mobile Engineering Sample (9 series)"},
	{0x8cc2, "Desktop Engineering Sample (9 series)"},
	{0x8cc3, "HM97"},
	{0x8cc4, "Z97"},
	{0x8cc6, "H97"},
	{0x9c41, "LP Full Featured Engineering Sample"},
	{0x9c43, "LP Premium"},
	{0x9c45, "LP Mainstream"},
	{0x9c47, "LP Value"},
};

static void report_pch_info(void)
{
	int i;
	u16 dev_id = pci_read_config16(PCH_LPC_DEV, 2);

	const char *pch_type = "Unknown";
	for (i = 0; i < ARRAY_SIZE(pch_table); i++) {
		if (pch_table[i].dev_id == dev_id) {
			pch_type = pch_table[i].dev_name;
			break;
		}
	}
	printk(BIOS_DEBUG, "PCH type: %s, device id: %x, rev id %x\n",
		pch_type, dev_id, pci_read_config8(PCH_LPC_DEV, 8));
}

void report_platform_info(void)
{
	report_cpu_info();
	report_pch_info();
}
