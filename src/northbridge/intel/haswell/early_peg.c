/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

static bool peg_hidden[MAX_PEG_FUNC];

static void start_peg2_link_training(const pci_devfn_t dev)
{
	u32 mask;

	switch (dev) {
	case PCI_DEV(0, 1, 2):
		mask = DEVEN_D1F2EN;
		break;
	case PCI_DEV(0, 1, 1):
		mask = DEVEN_D1F1EN;
		break;
	case PCI_DEV(0, 1, 0):
		mask = DEVEN_D1F0EN;
		break;
	default:
		printk(BIOS_ERR, "Link training tried on a non-PEG device!\n");
		return;
	}

	pci_update_config32(dev, 0xc24, ~(1 << 16), 1 << 5);
	printk(BIOS_DEBUG, "Started PEG1%d link training.\n", PCI_FUNC(PCI_DEV2DEVFN(dev)));

	/*
	 * The MRC will perform PCI enumeration, and if it detects a VGA
	 * device in a PEG slot, it will disable the IGD and not reserve
	 * any memory for it. Since the memory map is locked by the time
	 * MRC finishes, the IGD can't be enabled afterwards. Wonderful.
	 *
	 * If one really wants to enable the Intel iGPU as primary, hide
	 * all PEG devices during MRC execution. This will trick the MRC
	 * into thinking there aren't any, and will enable the IGD. Note
	 * that PEG AFE settings will not be programmed, which may cause
	 * stability problems at higher PCIe link speeds. The most ideal
	 * way to fix this problem for good is to implement native init.
	 */
	if (CONFIG(HASWELL_HIDE_PEG_FROM_MRC)) {
		pci_update_config32(HOST_BRIDGE, DEVEN, ~mask, 0);
		peg_hidden[PCI_FUNC(PCI_DEV2DEVFN(dev))] = true;
		printk(BIOS_DEBUG, "Temporarily hiding PEG1%d.\n",
		       PCI_FUNC(PCI_DEV2DEVFN(dev)));
	}
}

void northbridge_unhide_peg(void)
{
	u32 deven = pci_read_config32(HOST_BRIDGE, DEVEN);

	for (u8 fn = 0; fn < MAX_PEG_FUNC; fn++) {
		if (peg_hidden[fn]) {
			deven |= DEVEN_D1F0EN >> fn;
			peg_hidden[fn] = false;
			printk(BIOS_DEBUG, "Unhiding PEG1%d.\n", fn);
		}
	}

	pci_write_config32(HOST_BRIDGE, DEVEN, deven);
}

void northbridge_setup_peg(void)
{
	u32 deven = pci_read_config32(HOST_BRIDGE, DEVEN);

	if (deven & DEVEN_D1F2EN)
		start_peg2_link_training(PCI_DEV(0, 1, 2));

	if (deven & DEVEN_D1F1EN)
		start_peg2_link_training(PCI_DEV(0, 1, 1));

	if (deven & DEVEN_D1F0EN)
		start_peg2_link_training(PCI_DEV(0, 1, 0));
}
