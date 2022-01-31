/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <string.h>

#include <device/pci_ops.h>
#include <device/pci_def.h>

#include "gm45.h"

void init_iommu(void)
{
	/* FIXME: proper test? */
	int me_active = pci_read_config8(PCI_DEV(0, 3, 0), PCI_CLASS_REVISION) != 0xff;
	int stepping = pci_read_config8(PCI_DEV(0, 0, 0), PCI_CLASS_REVISION);

	mchbar_write32(0x28, IOMMU_BASE1 | 1); /* HDA @ 0:1b.0 */
	if (stepping != STEPPING_B2) {
		/* The official workaround is to run SMM every 64ms.
		   The only winning move is not to play. */
		mchbar_write32(0x18, IOMMU_BASE2 | 1); /* IGD @ 0:2.0-1 */
	} else {
		/* write-once, so lock it down */
		mchbar_write32(0x18, 0); /* disable IOMMU for IGD @ 0:2.0-1 */
	}
	if (me_active) {
		mchbar_write32(0x10, IOMMU_BASE3 | 1); /* ME  @ 0:3.0-3 */
	} else {
		mchbar_write32(0x10, 0); /* disable IOMMU for ME */
	}
	mchbar_write32(0x20, IOMMU_BASE4 | 1); /* all other DMA sources */

	/* clear GTT */
	u16 gtt = pci_read_config16(PCI_DEV(0, 0, 0), D0F0_GGC);
	if (gtt & 0x400) { /* VT mode */
		const pci_devfn_t igd = PCI_DEV(0, 2, 0);

		/* setup somewhere */
		pci_or_config16(igd, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
		void *bar = (void *)pci_read_config32(igd, PCI_BASE_ADDRESS_0);

		/* clear GTT, 2MB is enough (and should be safe) */
		memset(bar, 0, 2<<20);

		/* and now disable again */
		pci_and_config16(igd, PCI_COMMAND, ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY));
		pci_write_config32(igd, PCI_BASE_ADDRESS_0, 0);
	}

	if (stepping == STEPPING_B3) {
		mchbar_setbits8(0xffc, 1 << 4);
		const pci_devfn_t peg = PCI_DEV(0, 1, 0);

		/* FIXME: proper test? */
		if (pci_read_config8(peg, PCI_CLASS_REVISION) != 0xff)
			pci_or_config32(peg, 0xfc, 1 << 15);
	}

	/* final */
	mchbar_setbits8(0x94, 1 << 3);
}
