/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/pci_ids.h>

const struct cpu_power_limits limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, pl4 */
	/* All values are for baseline config as per bug:191906315 comment #10 */
	{ PCI_DEVICE_ID_INTEL_ADL_P_ID_7, 15, 3000, 15000, 39000, 39000, 100000 },
	{ PCI_DEVICE_ID_INTEL_ADL_P_ID_6, 15, 3000, 15000, 39000, 39000, 100000 },
	{ PCI_DEVICE_ID_INTEL_ADL_P_ID_5, 28, 4000, 28000, 43000, 43000, 105000 },
	{ PCI_DEVICE_ID_INTEL_ADL_P_ID_3, 28, 4000, 28000, 43000, 43000, 105000 },
	{ PCI_DEVICE_ID_INTEL_ADL_P_ID_3, 45, 5000, 45000, 80000, 80000, 159000 },
};

void variant_devtree_update(void)
{
	size_t total_entries = ARRAY_SIZE(limits);
	variant_update_power_limits(limits, total_entries);
}
