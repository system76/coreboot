/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>

#include <device/pci_ops.h>
#include <device/pci_def.h>

#include "gm45.h"

#define MEREMAPBAR	0x10
#define GFXREMAPBAR	0x18
#define VC0REMAPBAR	0x20
#define VC1REMAPBAR	0x28

void init_iommu(void)
{
	/* FIXME: proper test? */
	int me_active = pci_read_config8(PCI_DEV(0, 3, 0), PCI_CLASS_REVISION) != 0xff;
	int stepping = pci_read_config8(PCI_DEV(0, 0, 0), PCI_CLASS_REVISION);

	mchbar_write32(VC1REMAPBAR, IOMMU_BASE_VC1 | 1); /* HDA @ 0:1b.0 */
	if (stepping != STEPPING_B2) {
		/* The official workaround is to run SMM every 64ms.
		   The only winning move is not to play. */
		mchbar_write32(GFXREMAPBAR, IOMMU_BASE_GFX | 1); /* IGD @ 0:2.0-1 */
	} else {
		/* write-once, so lock it down */
		mchbar_write32(GFXREMAPBAR, 0); /* disable IOMMU for IGD @ 0:2.0-1 */
	}
	if (me_active) {
		mchbar_write32(MEREMAPBAR, IOMMU_BASE_ME | 1); /* ME  @ 0:3.0-3 */
	} else {
		mchbar_write32(MEREMAPBAR, 0); /* disable IOMMU for ME */
	}
	mchbar_write32(VC0REMAPBAR, IOMMU_BASE_VC0 | 1); /* all other DMA sources */

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
