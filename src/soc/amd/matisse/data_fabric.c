/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <console/console.h>
#include <cpu/x86/lapic_def.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/data_fabric.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <stdbool.h>

static void disable_mmio_reg(int reg)
{
	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(reg),
			   IOMS0_FABRIC_ID << MMIO_DST_FABRIC_ID_SHIFT);
	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_BASE(reg), 0);
	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(reg), 0);
}

static bool is_mmio_reg_disabled(int reg)
{
	uint32_t val = pci_read_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(reg));
	return !(val & ((MMIO_WE | MMIO_RE)));
}

static int find_unused_mmio_reg(void)
{
	unsigned int i;

	for (i = 0; i < NUM_NB_MMIO_REGS; i++) {
		if (is_mmio_reg_disabled(i))
			return i;
	}
	return -1;
}

struct data_fabric_range {
	uint8_t pci_base;
	uint8_t pci_limit;
	bool pci_read;
	bool pci_write;
	uint16_t pio_base;
	uint16_t pio_limit;
	bool pio_read;
	bool pio_write;
	uint64_t mmio_base;
	uint64_t mmio_limit;
	bool mmio_read;
	bool mmio_write;
};

// Basically do what AGESA should be doing. I/O memory is all of this space:
// PIO: 0x0000 - 0xFFFF
// MMIO: 0xC0000000 - 0xFFFFFFFF
const struct data_fabric_range RANGES[NUM_NB_MMIO_REGS] = {
	// DF0, Standard IO space, all read/write
	{
		.pci_base = 0x00,
		.pci_limit = 0xFF,
		.pci_read = true,
		.pci_write = true,
		.pio_base = 0x0000,
		.pio_limit = 0xFFFF,
		.pio_read = true,
		.pio_write = true,
		.mmio_base = 0xC0000000,
		.mmio_limit = 0xFFFFFFFF,
		.mmio_read = true,
		.mmio_write = true,
	},
	// DF1, Unset
	{0},
	// DF2, Unset
	{0},
	// DF3, Unset
	{0},
	// DF4, Unset
	{0},
	// DF5, Unset
	{0},
	// DF6, Unset
	{0},
	// DF7, Unset
	{0},
};

static void data_fabric_range_set(unsigned int reg, const struct data_fabric_range * range) {
	printk(BIOS_DEBUG, "data_fabric_range_set %d\n", reg);

	// Do not print while a range is disabled, you may lose access

	// PCI range
	{
		uint32_t map =
			(((uint32_t)range->pci_limit) << 24) |
			(((uint32_t)range->pci_base) << 16) |
			(IOMS0_FABRIC_ID << 4) |
			(((uint32_t)range->pci_write) << 1) |
			(((uint32_t)range->pci_read) << 0);
		printk(BIOS_DEBUG, "  pci map 0x%X\n", map);

		//TODO: Does it need to be disabled first?
		pci_write_config32(SOC_DF_F0_DEV, NB_PCI_MAP(reg), map);
	}

	// PIO range
	{
		uint32_t base =
			(((uint32_t)range->pio_base) << 12) |
			(((uint32_t)range->pio_write) << 1) |
			(((uint32_t)range->pio_read) << 0);
		uint32_t limit =
			(((uint32_t)range->pio_limit) << 12) |
			IOMS0_FABRIC_ID;
		printk(BIOS_DEBUG, "  pio base 0x%X\n", base);
		printk(BIOS_DEBUG, "  pio limit 0x%X\n", limit);

		//TODO: Does it need to be disabled first?
		pci_write_config32(SOC_DF_F0_DEV, NB_PIO_BASE(reg), base);
		pci_write_config32(SOC_DF_F0_DEV, NB_PIO_LIMIT(reg), limit);
	}

	// MMIO range
	{
		//TODO: make sure mmio_base and mmio_limit have no invalid bits
		uint32_t base =
			(uint32_t)(range->mmio_base >> D18F0_MMIO_SHIFT);
		uint32_t limit =
			(uint32_t)(range->mmio_limit >> D18F0_MMIO_SHIFT);
		uint32_t ctrl =
			(((uint32_t)range->mmio_write) << 1) |
			(((uint32_t)range->mmio_read) << 0) |
			(IOMS0_FABRIC_ID << MMIO_DST_FABRIC_ID_SHIFT);

		printk(BIOS_DEBUG, "  mmio base 0x%X\n", base);
		printk(BIOS_DEBUG, "  mmio limit 0x%X\n", limit);
		printk(BIOS_DEBUG, "  mmio ctrl 0x%X\n", ctrl);

		// Disable the mmio range
		pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(reg), IOMS0_FABRIC_ID << MMIO_DST_FABRIC_ID_SHIFT);

		// Set base and limit
		pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_BASE(reg), base);
		pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(reg), limit);

		// Enable the range
		pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(reg), ctrl);
	}
}

static void data_fabric_init(void) {
	printk(BIOS_DEBUG, "data_fabric_init start\n");

	for (unsigned int reg = 0; reg < NUM_NB_MMIO_REGS; reg++) {
		data_fabric_range_set(reg, &RANGES[reg]);
	}

	printk(BIOS_DEBUG, "data_fabric_init done\n");
}

void data_fabric_set_mmio_np(void)
{
	/*
	 * Mark region from HPET-LAPIC or 0xfed00000-0xfee00000-1 as NP.
	 *
	 * AGESA has already programmed the NB MMIO routing, however nothing
	 * is yet marked as non-posted.
	 *
	 * If there exists an overlapping routing base/limit pair, trim its
	 * base or limit to avoid the new NP region.  If any pair exists
	 * completely within HPET-LAPIC range, remove it.  If any pair surrounds
	 * HPET-LAPIC, it must be split into two regions.
	 *
	 * TODO(b/156296146): Remove the settings from AGESA and allow coreboot
	 * to own everything.  If not practical, consider erasing all settings
	 * and have coreboot reprogram them.  At that time, make the source
	 * below more flexible.
	 *   * Note that the code relies on the granularity of the HPET and
	 *     LAPIC addresses being sufficiently large that the shifted limits
	 *     +/-1 are always equivalent to the non-shifted values +/-1.
	 */
	printk(BIOS_DEBUG, "data_fabric_set_mmio_np start\n");

	data_fabric_init();

	unsigned int i;
	int reg;
	uint32_t base, limit, ctrl;
	const uint32_t np_bot = HPET_BASE_ADDRESS >> D18F0_MMIO_SHIFT;
	const uint32_t np_top = (LOCAL_APIC_ADDR - 1) >> D18F0_MMIO_SHIFT;
	printk(BIOS_DEBUG, " np_bot: 0x%X, np_top: 0x%X\n", np_bot, np_top);

	for (i = 0; i < NUM_NB_MMIO_REGS; i++) {
		printk(BIOS_DEBUG, " df %d\n", i);

		/* Adjust all registers that overlap */
		ctrl = pci_read_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(i));
		printk(BIOS_DEBUG, "  ctrl 0x%X\n", ctrl);
		if (!(ctrl & (MMIO_WE | MMIO_RE)))
			continue; /* not enabled */

		base = pci_read_config32(SOC_DF_F0_DEV, NB_MMIO_BASE(i));
		printk(BIOS_DEBUG, "  base 0x%X\n", base);
		limit = pci_read_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(i));
		printk(BIOS_DEBUG, "  limit 0x%X\n", limit);

		if (base > np_top || limit < np_bot) {
			printk(BIOS_DEBUG, "  no overlap at all\n");
			continue; /* no overlap at all */
		}

		if (base >= np_bot && limit <= np_top) {
			printk(BIOS_DEBUG, "  completely within, so remove\n");
			disable_mmio_reg(i); /* 100% within, so remove */
			continue;
		}

		if (base < np_bot && limit > np_top) {
			printk(BIOS_DEBUG, "  split the region\n");

			/* Split the configured region */
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(i), np_bot - 1);
			reg = find_unused_mmio_reg();
			if (reg < 0) {
				/* Although a pair could be freed later, this condition is
				 * very unusual and deserves analysis.  Flag an error and
				 * leave the topmost part unconfigured. */
				printk(BIOS_ERR,
				       "Error: Not enough NB MMIO routing registers\n");
				continue;
			}
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_BASE(reg), np_top + 1);
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(reg), limit);
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(reg), ctrl);
			continue;
		}

		/* If still here, adjust only the base or limit */
		if (base <= np_bot) {
			printk(BIOS_DEBUG, "  adjust limit\n");
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(i), np_bot - 1);
		} else {
			printk(BIOS_DEBUG, "  adjust base\n");
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_BASE(i), np_top + 1);
		}
	}

	reg = find_unused_mmio_reg();
	printk(BIOS_DEBUG, " find_unused_mmio_reg: 0x%X\n", reg);
	if (reg < 0) {
		printk(BIOS_ERR, "Error: cannot configure region as NP\n");
		return;
	}

	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_BASE(reg), np_bot);
	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(reg), np_top);
	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(reg),
			   (IOMS0_FABRIC_ID << MMIO_DST_FABRIC_ID_SHIFT) | MMIO_NP | MMIO_WE
				   | MMIO_RE);

	printk(BIOS_DEBUG, "data_fabric_set_mmio_np finish\n");
}

static const char *data_fabric_acpi_name(const struct device *dev)
{
	switch (dev->device) {
	case 0x1440:
		return "DFD0";
	case 0x1441:
		return "DFD1";
	case 0x1442:
		return "DFD2";
	case 0x1443:
		return "DFD3";
	case 0x1444:
		return "DFD4";
	case 0x1445:
		return "DFD5";
	case 0x1446:
		return "DFD6";
	case 0x1447:
		return "DFD7";
	default:
		printk(BIOS_ERR, "%s: Unhandled device id 0x%x\n", __func__, dev->device);
	}

	return NULL;
}

static struct device_operations data_fabric_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= data_fabric_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};

static const unsigned short pci_device_ids[] = {
	0x1440,
	0x1441,
	0x1442,
	0x1443,
	0x1444,
	0x1445,
	0x1446,
	0x1447,
	0
};

static const struct pci_driver data_fabric_driver __pci_driver = {
	.ops			= &data_fabric_ops,
	.vendor			= PCI_VENDOR_ID_AMD,
	.devices		= pci_device_ids,
};
