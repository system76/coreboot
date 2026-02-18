/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8772f/it8772f.h>

#define IT8772F_BASE	0x2e
#define EC_DEV		PNP_DEV(IT8772F_BASE, IT8772F_EC)
#define GPIO_DEV	PNP_DEV(IT8772F_BASE, IT8772F_GPIO)

void bootblock_mainboard_early_init(void)
{
	pci_write_config16(PCH_LPC_DEV, LPC_EN, 0x3c0f);
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0010);

	ite_disable_pme_out(EC_DEV);
	ite_ac_resume_southbridge(EC_DEV);
	ite_disable_3vsbsw(GPIO_DEV);
	ite_kill_watchdog(GPIO_DEV);
}
