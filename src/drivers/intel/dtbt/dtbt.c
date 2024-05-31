/* SPDX-License-Identifier: GPL-2.0-only */

#include "chip.h"
#include <acpi/acpigen.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <timer.h>

#define PCIE2TBT 0x54C
#define PCIE2TBT_VALID BIT(0)
#define PCIE2TBT_GO2SX			2
#define PCIE2TBT_GO2SX_NO_WAKE		3
#define PCIE2TBT_SX_EXIT_TBT_CONNECTED	4
#define PCIE2TBT_OS_UP			6
#define PCIE2TBT_SET_SECURITY_LEVEL	8
#define PCIE2TBT_GET_SECURITY_LEVEL	9
#define PCIE2TBT_BOOT_ON		24
#define PCIE2TBT_USB_ON			25
#define PCIE2TBT_GET_ENUMERATION_METHOD	26
#define PCIE2TBT_SET_ENUMERATION_METHOD	27
#define PCIE2TBT_POWER_CYCLE		28
#define PCIE2TBT_SX_START		29
#define PCIE2TBT_ACL_BOOT		30
#define PCIE2TBT_CONNECT_TOPOLOGY	31

#define TBT2PCIE 0x548
#define TBT2PCIE_DONE BIT(0)

// Default timeout for mailbox commands unless otherwise specified.
#define TIMEOUT_MS 1000
// Default timeout for controller to ack GO2SX/GO2SX_NO_WAKE mailbox command.
#define GO2SX_TIMEOUT_MS 600

static void dtbt_cmd(struct device *dev, u32 command, u32 data, u32 timeout)
{
	u32 reg = (data << 8) | (command << 1) | PCIE2TBT_VALID;
	u32 status;

	printk(BIOS_DEBUG, "dTBT send command %08x\n", command);
	pci_write_config32(dev, PCIE2TBT, reg);

	if (!wait_ms(timeout, (status = pci_read_config32(dev, TBT2PCIE)) & TBT2PCIE_DONE)) {
		printk(BIOS_ERR, "dTBT command %08x send timeout %08x\n", command, status);
	}

	pci_write_config32(dev, PCIE2TBT, 0);
	if (!wait_ms(timeout, !(pci_read_config32(dev, TBT2PCIE) & TBT2PCIE_DONE))) {
		printk(BIOS_ERR, "dTBT command %08x clear timeout\n", command);
	}
}

static void dtbt_write_dsd(void)
{
	struct acpi_dp *dsd = acpi_dp_new_table("_DSD");

	acpi_device_add_hotplug_support_in_d3(dsd);
	acpi_device_add_external_facing_port(dsd);
	acpi_dp_write(dsd);
}

static void dtbt_write_opregion(const struct bus *bus)
{
	uintptr_t mmconf_base = (uintptr_t)CONFIG_ECAM_MMCONF_BASE_ADDRESS
			+ (((uintptr_t)(bus->secondary)) << 20);
	const struct opregion opregion = OPREGION("PXCS", SYSTEMMEMORY, mmconf_base, 0x1000);
	const struct fieldlist fieldlist[] = {
		FIELDLIST_OFFSET(TBT2PCIE),
		FIELDLIST_NAMESTR("TB2P", 32),
		FIELDLIST_OFFSET(PCIE2TBT),
		FIELDLIST_NAMESTR("P2TB", 32),
	};

	acpigen_write_opregion(&opregion);
	acpigen_write_field("PXCS", fieldlist, ARRAY_SIZE(fieldlist),
			    FIELD_DWORDACC | FIELD_NOLOCK | FIELD_PRESERVE);
}

static void dtbt_fill_ssdt(const struct device *dev)
{
	struct bus *bus;
	struct device *parent;
	const char *parent_scope;
	const char *dev_name = acpi_device_name(dev);

	bus = dev->upstream;
	if (!bus) {
		printk(BIOS_ERR, "dTBT bus invalid\n");
		return;
	}

	parent = bus->dev;
	if (!parent || parent->path.type != DEVICE_PATH_PCI) {
		printk(BIOS_ERR, "dTBT parent invalid\n");
		return;
	}

	parent_scope = acpi_device_path(parent);
	if (!parent_scope) {
		printk(BIOS_ERR, "dTBT parent scope not valid\n");
		return;
	}

	/* Scope */
	acpigen_write_scope(parent_scope);
	dtbt_write_dsd();

	/* Device */
	acpigen_write_device(dev_name);
	acpigen_write_name_integer("_ADR", 0);
	dtbt_write_opregion(bus);

	/* Method */
	acpigen_write_method_serialized("PTS", 0);

	acpigen_write_debug_string("dTBT prepare to sleep");
	acpigen_write_store_int_to_namestr(PCIE2TBT_GO2SX_NO_WAKE << 1, "P2TB");
	acpigen_write_delay_until_namestr_int(GO2SX_TIMEOUT_MS, "TB2P", PCIE2TBT_GO2SX_NO_WAKE << 1);

	acpigen_write_debug_namestr("TB2P");
	acpigen_write_store_int_to_namestr(0, "P2TB");
	acpigen_write_delay_until_namestr_int(GO2SX_TIMEOUT_MS, "TB2P", 0);
	acpigen_write_debug_namestr("TB2P");

	acpigen_write_method_end();
	acpigen_write_device_end();
	acpigen_write_scope_end();

	printk(BIOS_DEBUG, "dTBT fill SSDT\n");
	printk(BIOS_DEBUG, "  Dev %s\n", dev_path(dev));
	//printk(BIOS_DEBUG, "  Bus %s\n", bus_path(bus));
	printk(BIOS_DEBUG, "  Parent %s\n", dev_path(parent));
	printk(BIOS_DEBUG, "  Scope %s\n", parent_scope);
	printk(BIOS_DEBUG, "    Device %s\n", dev_name);

	// \.TBTS Method
	acpigen_write_scope("\\");
	acpigen_write_method("TBTS", 0);
	acpigen_emit_namestring(acpi_device_path_join(dev, "PTS"));
	acpigen_write_method_end();
	acpigen_write_scope_end();
}

static const char *dtbt_acpi_name(const struct device *dev)
{
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
	.acpi_name        = dtbt_acpi_name,
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

	// TODO: check device ID

	dev->ops = &dtbt_device_ops;

	printk(BIOS_INFO, "dTBT controller found at %s\n", dev_path(dev));

	// XXX: Recommendation is to set SL1 ("User Authorization")
	printk(BIOS_DEBUG, "dTBT set security level SL0\n");
	dtbt_cmd(dev, PCIE2TBT_SET_SECURITY_LEVEL, 0, TIMEOUT_MS);
	// XXX: Must verify change or rollback all controllers

	if (acpi_is_wakeup_s3()) {
		printk(BIOS_DEBUG, "dTBT SX exit\n");
		dtbt_cmd(dev, PCIE2TBT_SX_EXIT_TBT_CONNECTED, 0, TIMEOUT_MS);
		// TODO: "wait for fast link bring-up" loop (timeout: 5s)
	} else {
		printk(BIOS_DEBUG, "dTBT boot on\n");
		dtbt_cmd(dev, PCIE2TBT_BOOT_ON, 0, TIMEOUT_MS);
	}
}

struct chip_operations drivers_intel_dtbt_ops = {
	.name = "Intel Discrete Thunderbolt",
	.enable_dev = dtbt_enable,
};
