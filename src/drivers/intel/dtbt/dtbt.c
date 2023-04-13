/* SPDX-License-Identifier: GPL-2.0-only */

#include "chip.h"
#include <acpi/acpigen.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>

#define PCIE2TBT 0x54C
#define PCIE2TBT_SET_SECURITY_LEVEL ((0x08 << 1) | 1)
#define PCIE2TBT_GET_SECURITY_LEVEL ((0x09 << 1) | 1)
#define PCIE2TBT_BOOT_ON ((0x18 << 1) | 1)
#define PCIE2TBT_CONNECT_TOPOLOGY ((0x1F << 1) | 1)
#define PCIE2TBT_FIRMWARE_CM_MODE ((0x22 << 1) | 1)
#define TBT2PCIE 0x548

static void dtbt_cmd(struct device *dev, u32 command) {
	printk(BIOS_INFO, "DTBT send command %08x\n", command);

	pci_write_config32(dev, PCIE2TBT, command);

	u32 timeout;
	u32 status;
	for (timeout = 1000000; timeout > 0; timeout--) {
		status = pci_read_config32(dev, TBT2PCIE);
		if (status & 1) {
			break;
		}
		udelay(1);
	}
	if (timeout == 0) {
		printk(BIOS_ERR, "DTBT command %08x timeout on status %08x\n", command, status);
	}

	printk(BIOS_ERR, "DTBT command %08x status %08x\n", command, status);

	pci_write_config32(dev, PCIE2TBT, 0);

	u32 status_clear;
	for (timeout = 1000000; timeout > 0; timeout--) {
		status_clear = pci_read_config32(dev, TBT2PCIE);
		if (!(status_clear & 1)) {
			break;
		}
		udelay(1);
	}
	if (timeout == 0) {
		printk(BIOS_ERR, "DTBT command %08x timeout on status clear %08x\n", command, status_clear);
	}
}

static void dtbt_fill_ssdt(const struct device *dev) {
	printk(BIOS_INFO, "DTBT fill SSDT\n");

	const char *dev_scope = acpi_device_path(dev->bus->dev);
	if (!dev_scope) {
		return;
	}

	{ /* Scope */
		printk(BIOS_INFO, "  Scope %s\n", dev_scope);
		acpigen_write_scope(dev_scope);

		struct acpi_dp *dsd = acpi_dp_new_table("_DSD");

		/* Indicate that device supports hotplug in D3. */
		acpi_device_add_hotplug_support_in_d3(dsd);

		/* Indicate that port is external. */
		acpi_device_add_external_facing_port(dsd);

		acpi_dp_write(dsd);

		{ /* Device */
			const char *dev_name = acpi_device_name(dev);
			printk(BIOS_INFO, "    Device %s\n", dev_name);
			acpigen_write_device(dev_name);

			acpigen_write_name_integer("_ADR", 0);

			acpigen_write_device_end();
		}

		acpigen_write_scope_end();
	}
}

static const char *dtbt_acpi_name(const struct device *dev) {
	return "DTBT";
}

static struct pci_operations dtbt_device_ops_pci = {
	.set_subsystem = 0,
};

static struct device_operations dtbt_device_ops = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.acpi_fill_ssdt   = dtbt_fill_ssdt,
	.acpi_name 		  = dtbt_acpi_name,
	.scan_bus         = pciexp_scan_bridge,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &dtbt_device_ops_pci,
};

static void dtbt_enable(struct device *dev)
{
	if (!is_dev_enabled(dev) || dev->path.type != DEVICE_PATH_PCI)
		return;

	if (pci_read_config16(dev, PCI_VENDOR_ID) != PCI_VID_INTEL)
		return;

	//TODO: check device ID

	dev->ops = &dtbt_device_ops;

	printk(BIOS_INFO, "DTBT controller found at %s\n", dev_path(dev));

	printk(BIOS_INFO, "DTBT get security level\n");
	dtbt_cmd(dev, PCIE2TBT_GET_SECURITY_LEVEL);

	printk(BIOS_INFO, "DTBT set security level SL0\n");
	dtbt_cmd(dev, PCIE2TBT_SET_SECURITY_LEVEL);

	printk(BIOS_INFO, "DTBT get security level\n");
	dtbt_cmd(dev, PCIE2TBT_GET_SECURITY_LEVEL);

	printk(BIOS_INFO, "DTBT boot on\n");
	dtbt_cmd(dev, PCIE2TBT_BOOT_ON);
}

struct chip_operations drivers_intel_dtbt_ops = {
	CHIP_NAME("Intel Discrete Thunderbolt Device")
	.enable_dev = dtbt_enable
};
