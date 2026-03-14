/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/cpu_ids.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/name.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <intelblocks/acpi.h>
#include <intelblocks/car_lib.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pmclib.h>
#include <soc/bootblock.h>
#include <soc/pci_devs.h>

static struct {
	u32 cpuid;
	const char *name;
} cpu_table[] = {
	{ CPUID_NOVALAKE, "Nova Lake" },
};

struct device_name_map {
	u16 id;
	const char *name;
};

static struct device_name_map mch_table[] = {
	{ PCI_DID_INTEL_NVL_ID_1, "Nova Lake" },
	{ PCI_DID_INTEL_NVL_ID_2, "Nova Lake" },
	{ PCI_DID_INTEL_NVL_ID_3, "Nova Lake" },
	{ PCI_DID_INTEL_NVL_ID_4, "Nova Lake" },
};

static struct device_name_map pch_table[] = {
	{ PCI_DID_INTEL_NVL_ESPI_0, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_1, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_2, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_3, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_4, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_5, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_6, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_7, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_8, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_9, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_10, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_11, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_12, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_13, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_14, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_15, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_16, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_17, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_18, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_19, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_20, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_21, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_22, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_23, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_24, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_25, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_26, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_27, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_28, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_29, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_30, "Nova Lake SOC" },
	{ PCI_DID_INTEL_NVL_ESPI_31, "Nova Lake SOC" },
};

static struct device_name_map igd_table[] = {
	{ PCI_DID_INTEL_NVL_GT2_1, "Nova Lake GT2" },
	{ PCI_DID_INTEL_NVL_GT2_2, "Nova Lake GT2" },
	{ PCI_DID_INTEL_NVL_GT2_3, "Nova Lake GT2" },
	{ PCI_DID_INTEL_NVL_GT2_4, "Nova Lake GT2" },
	{ PCI_DID_INTEL_NVL_GT2_5, "Nova Lake GT2" },
};

static inline uint8_t get_dev_revision(pci_devfn_t dev)
{
	return pci_read_config8(dev, PCI_REVISION_ID);
}

static inline uint16_t get_dev_id(pci_devfn_t dev)
{
	return pci_read_config16(dev, PCI_DEVICE_ID);
}

static void report_cpu_info(void)
{
	u32 i, cpu_id, cpu_feature_flag;
	char cpu_name[49];
	int vt, txt, aes;
	static const char *const mode[] = {"NOT ", ""};
	const char *cpu_type = "Unknown";

	fill_processor_name(cpu_name);
	cpu_id = cpu_get_cpuid();

	/* Look for string to match the name */
	for (i = 0; i < ARRAY_SIZE(cpu_table); i++) {
		if (cpuid_match(cpu_table[i].cpuid, cpu_id, CPUID_ALL_STEPPINGS_MASK)) {
			cpu_type = cpu_table[i].name;
			break;
		}
	}

	printk(BIOS_DEBUG, "CPU: %s\n", cpu_name);
	printk(BIOS_DEBUG, "CPU: ID %x, %s, ucode: %08x\n",
	       cpu_id, cpu_type, get_current_microcode_rev());

	cpu_feature_flag = cpu_get_feature_flags_ecx();
	aes = !!(cpu_feature_flag & CPUID_AES);
	txt = !!(cpu_feature_flag & CPUID_SMX);
	vt = !!(cpu_feature_flag & CPUID_VMX);
	printk(BIOS_DEBUG,
		"CPU: AES %ssupported, TXT %ssupported, VT %ssupported\n",
		mode[aes], mode[txt], mode[vt]);

	car_report_cache_info();
	pmc_dump_soc_qdf_info();
}

static const char *find_device_name(const struct device_name_map *table,
				    size_t count, u16 id)
{
	for (size_t i = 0; i < count; i++) {
		if (table[i].id == id)
			return table[i].name;
	}
	return "Unknown";
}

static void report_dev_info(const char *prefix, pci_devfn_t dev,
			    const struct device_name_map *table, size_t count)
{
	uint16_t devid = get_dev_id(dev);
	printk(BIOS_DEBUG, "%s: device id %04x (rev %02x) is %s\n",
	       prefix, devid, get_dev_revision(dev),
	       find_device_name(table, count, devid));
}

static void report_mch_info(void)
{
	report_dev_info("MCH", PCI_DEV_ROOT, mch_table, ARRAY_SIZE(mch_table));
}

static void report_pch_info(void)
{
	report_dev_info("PCH", PCI_DEV_ESPI, pch_table, ARRAY_SIZE(pch_table));
}

static void report_igd_info(void)
{
	report_dev_info("IGD", PCI_DEV_IGD, igd_table, ARRAY_SIZE(igd_table));
}

void report_platform_info(void)
{
	report_cpu_info();
	report_mch_info();
	report_pch_info();
	report_igd_info();
}
