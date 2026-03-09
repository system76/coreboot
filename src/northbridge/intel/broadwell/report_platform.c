/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/systemagent.h>
#include <string.h>

static struct {
	u8 revid;
	const char *name;
} mch_rev_table[] = {
	{ MCH_BROADWELL_REV_D0, "Broadwell D0" },
	{ MCH_BROADWELL_REV_E0, "Broadwell E0" },
	{ MCH_BROADWELL_REV_F0, "Broadwell F0" },
};

static struct {
	u16 lpcid;
	const char *name;
} pch_table[] = {
	{ PCH_LPT_LP_SAMPLE,     "LynxPoint LP Sample" },
	{ PCH_LPT_LP_PREMIUM,    "LynxPoint LP Premium" },
	{ PCH_LPT_LP_MAINSTREAM, "LynxPoint LP Mainstream" },
	{ PCH_LPT_LP_VALUE,      "LynxPoint LP Value" },
	{ PCH_WPT_HSW_U_SAMPLE,  "Haswell U Sample" },
	{ PCH_WPT_BDW_U_SAMPLE,  "Broadwell U Sample" },
	{ PCH_WPT_BDW_U_PREMIUM, "Broadwell U Premium" },
	{ PCH_WPT_BDW_U_BASE,    "Broadwell U Base" },
	{ PCH_WPT_BDW_Y_SAMPLE,  "Broadwell Y Sample" },
	{ PCH_WPT_BDW_Y_PREMIUM, "Broadwell Y Premium" },
	{ PCH_WPT_BDW_Y_BASE,    "Broadwell Y Base" },
	{ PCH_WPT_BDW_H,         "Broadwell H" },
};

static struct {
	u16 igdid;
	const char *name;
} igd_table[] = {
	{ IGD_HASWELL_ULT_GT1,     "Haswell ULT GT1" },
	{ IGD_HASWELL_ULT_GT2,     "Haswell ULT GT2" },
	{ IGD_HASWELL_ULT_GT3,     "Haswell ULT GT3" },
	{ IGD_BROADWELL_U_GT1,     "Broadwell U GT1" },
	{ IGD_BROADWELL_U_GT2,     "Broadwell U GT2" },
	{ IGD_BROADWELL_U_GT3_15W, "Broadwell U GT3 (15W)" },
	{ IGD_BROADWELL_U_GT3_28W, "Broadwell U GT3 (28W)" },
	{ IGD_BROADWELL_Y_GT2,     "Broadwell Y GT2" },
	{ IGD_BROADWELL_H_GT2,     "Broadwell U GT2" },
	{ IGD_BROADWELL_H_GT3,     "Broadwell U GT3" },
};

static void report_mch_info(void)
{
	const u16 mch_device = pci_read_config16(HOST_BRIDGE, PCI_DEVICE_ID);
	const u8 mch_revision = pci_read_config8(HOST_BRIDGE, PCI_REVISION_ID);
	const char *mch_type = "Unknown";

	/* Look for string to match the revision for Broadwell U/Y */
	if (mch_device == MCH_BROADWELL_ID_U_Y) {
		for (unsigned int i = 0; i < ARRAY_SIZE(mch_rev_table); i++) {
			if (mch_rev_table[i].revid == mch_revision) {
				mch_type = mch_rev_table[i].name;
				break;
			}
		}
	}

	printk(BIOS_DEBUG, "MCH: device id %04x (rev %02x) is %s\n",
	       mch_device, mch_revision, mch_type);
}

static void report_pch_info(void)
{
	const u16 lpcid = pch_type();
	const char *pch_type = "Unknown";

	for (unsigned int i = 0; i < ARRAY_SIZE(pch_table); i++) {
		if (pch_table[i].lpcid == lpcid) {
			pch_type = pch_table[i].name;
			break;
		}
	}
	printk(BIOS_DEBUG, "PCH: device id %04x (rev %02x) is %s\n",
	       lpcid, pch_revision(), pch_type);
}

static void report_igd_info(void)
{
	const u16 igdid = pci_read_config16(SA_DEV_IGD, PCI_DEVICE_ID);
	const char *igd_type = "Unknown";

	for (unsigned int i = 0; i < ARRAY_SIZE(igd_table); i++) {
		if (igd_table[i].igdid == igdid) {
			igd_type = igd_table[i].name;
			break;
		}
	}
	printk(BIOS_DEBUG, "IGD: device id %04x (rev %02x) is %s\n",
	       igdid, pci_read_config8(SA_DEV_IGD, PCI_REVISION_ID), igd_type);
}

void report_platform_info(void)
{
	report_cpu_info();
	report_mch_info();
	report_pch_info();
	report_igd_info();
}
