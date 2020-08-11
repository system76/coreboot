/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/data_fabric.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include "chip.h"

#if CONFIG(PLATFORM_USES_FSP2_0)
#include <fsp/api.h>
#endif

/* Supplied by i2c.c */
extern struct device_operations matisse_i2c_mmio_ops;
/* Supplied by uart.c */
extern struct device_operations matisse_uart_mmio_ops;

struct device_operations cpu_bus_ops = {
	.read_resources	  = noop_read_resources,
	.set_resources	  = noop_set_resources,
	.init		  = mp_cpu_bus_init,
	.acpi_fill_ssdt   = generate_cpu_entries,
};

const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	if (dev->bus->dev->path.type == DEVICE_PATH_DOMAIN) {
		switch (dev->path.pci.devfn) {
		case GNB_DEVFN:
			return "GNB";
		case IOMMU_DEVFN:
			return "IOMM";
		case LPC_DEVFN:
			return "LPCB";
		case SMBUS_DEVFN:
			return "SBUS";
		default:
			printk(BIOS_WARNING, "Unknown root PCI device: dev: %d, fn: %d\n",
			       PCI_SLOT(dev->path.pci.devfn), PCI_FUNC(dev->path.pci.devfn));
			return NULL;
		}
	}

	printk(BIOS_WARNING, "Unknown PCI device: dev: %d, fn: %d\n",
	       PCI_SLOT(dev->path.pci.devfn), PCI_FUNC(dev->path.pci.devfn));
	return NULL;
};

struct device_operations pci_domain_ops = {
	.read_resources	  = pci_domain_read_resources,
	.set_resources	  = pci_domain_set_resources,
	.scan_bus	  = pci_domain_scan_bus,
	.acpi_name	  = soc_acpi_name,
};

static void set_mmio_dev_ops(struct device *dev)
{
	switch (dev->path.mmio.addr) {
	case APU_I2C0_BASE:
	case APU_I2C1_BASE:
	case APU_I2C2_BASE:
	case APU_I2C3_BASE:
	case APU_I2C4_BASE:
	case APU_I2C5_BASE:
		dev->ops = &matisse_i2c_mmio_ops;
		break;
	case APU_UART0_BASE:
	case APU_UART1_BASE:
		dev->ops = &matisse_uart_mmio_ops;
		break;
	}
}

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_MMIO) {
		set_mmio_dev_ops(dev);
	}
}

// SMN pair for BIOS use (SMN 2)
#define SMN_INDEX 0xB8
#define SMN_DATA 0xBC

static uint32_t smn_read(uint32_t address) {
	pci_write_config32(SOC_GNB_DEV, SMN_INDEX, address);
	return pci_read_config32(SOC_GNB_DEV, SMN_DATA);
}

static void smn_write(uint32_t address, uint32_t value) {
	pci_write_config32(SOC_GNB_DEV, SMN_INDEX, address);
	pci_write_config32(SOC_GNB_DEV, SMN_DATA, value);
}

struct pcie_core {
	uint32_t smn_base;
};

const struct pcie_core PCIE_CORES[] = {
	{
		.smn_base = 0x11480000,
	},
	{
		.smn_base = 0x11880000,
	}
};

static void pcie_init(void) {
	printk(BIOS_DEBUG, "pcie_init start\n");

	printk(BIOS_DEBUG, "PCIE0 SWRST_COMMAND_STATUS 0x%X\n", smn_read(0x11480400));
	printk(BIOS_DEBUG, "PCIE0 SWRST_CONTROL_6 0x%X\n", smn_read(0x11480428));

	printk(BIOS_DEBUG, "PCIE1 SWRST_COMMAND_STATUS 0x%X\n", smn_read(0x11880400));
	printk(BIOS_DEBUG, "PCIE1 SWRST_CONTROL_6 0x%X\n", smn_read(0x11880428));

	int i;

#if 0
	// Wait for all cores to reset
	printk(BIOS_DEBUG, "pcie_init reset\n");
	for (i = 0; i < ARRAY_SIZE(PCIE_CORES); i++) {
		const struct pcie_core * core = &PCIE_CORES[i];

		// Wait until RESET_COMPLETE is set
		while (!(smn_read(core->smn_base + 0x400) & (1 << 16))) {}
	}

	//TODO: set straps using RSMU index/data pair method

	// Reconfigure to make strap changes take effect
	printk(BIOS_DEBUG, "pcie_init reconfigure\n");
	for (i = 0; i < ARRAY_SIZE(PCIE_CORES); i++) {
		const struct pcie_core * core = &PCIE_CORES[i];

		// Set RECONFIGURE
		smn_write(core->smn_base + 0x400, smn_read(core->smn_base + 0x400) | (1 << 0));

		// Wait until RECONFIGURE is cleared
		while (smn_read(core->smn_base + 0x400) & (1 << 0)) {}
	}
#endif

	//Let's allow link training, just for fun
	printk(BIOS_DEBUG, "pcie_init training\n");
	for (i = 0; i < ARRAY_SIZE(PCIE_CORES); i++) {
		const struct pcie_core * core = &PCIE_CORES[i];

		// Clear HOLD_TRAINING_*
		smn_write(core->smn_base + 0x428, 0);
	}

	printk(BIOS_DEBUG, "PCIE0 SWRST_COMMAND_STATUS 0x%X\n", smn_read(0x11480400));
	printk(BIOS_DEBUG, "PCIE0 SWRST_CONTROL_6 0x%X\n", smn_read(0x11480428));

	printk(BIOS_DEBUG, "PCIE1 SWRST_COMMAND_STATUS 0x%X\n", smn_read(0x11880400));
	printk(BIOS_DEBUG, "PCIE1 SWRST_CONTROL_6 0x%X\n", smn_read(0x11880428));

	printk(BIOS_DEBUG, "pcie_init finish\n");
}

static void soc_init(void *chip_info)
{
	printk(BIOS_DEBUG, "soc_init start\n");

	default_dev_ops_root.write_acpi_tables = agesa_write_acpi_tables;

	data_fabric_init();

	pcie_init();

	data_fabric_set_mmio_np();
	southbridge_init(chip_info);

	printk(BIOS_DEBUG, "soc_init finish\n");
}

static void soc_final(void *chip_info)
{
	southbridge_final(chip_info);
}

struct chip_operations soc_amd_matisse_ops = {
	CHIP_NAME("AMD Matisse SOC")
	.enable_dev = enable_dev,
	.init = soc_init,
	.final = soc_final
};
