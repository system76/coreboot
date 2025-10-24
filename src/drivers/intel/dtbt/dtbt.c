/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <timer.h>
#include "chip.h"
#include "dtbt.h"


/*
 * We only want to enable the first/primary bridge device,
 * as sending mailbox commands to secondary ones will fail,
 * and we only want to create a single ACPI device in the SSDT.
 */
static bool enable_done;
static bool ssdt_done;

static void dtbt_cmd(struct device *dev, u32 command, u32 data, u32 timeout)
{
	u32 reg = (data << 8) | (command << 1) | PCIE2TBT_VALID;
	u32 status;

	printk(BIOS_SPEW, "dTBT send command 0x%x\n", command);
	/* Send command */
	pci_write_config32(dev, PCIE2TBT, reg);
	/* Wait for done bit to be cleared */
	if (!wait_ms(timeout, (status = pci_read_config32(dev, TBT2PCIE)) & TBT2PCIE_DONE))
		printk(BIOS_ERR, "dTBT command 0x%x send timeout, status 0x%x\n", command, status);
	/* Clear valid bit */
	pci_write_config32(dev, PCIE2TBT, 0);
	/* Wait for done bit to be cleared */
	if (!wait_ms(timeout, (status = pci_read_config32(dev, TBT2PCIE)) & TBT2PCIE_DONE))
		printk(BIOS_ERR, "dTBT command 0x%x clear valid bit timeout, status 0x%x\n", command, status);
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

	if (ssdt_done)
		return;

	bus = dev->upstream;
	if (!bus) {
		printk(BIOS_ERR, "dTBT bus invalid\n");
		return;
	}

	parent = bus->dev;
	if (!parent || !is_pci(parent)) {
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

	/* PTS Method */
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

	// \.TBTS Method
	acpigen_write_scope("\\");
	acpigen_write_method("TBTS", 0);
	acpigen_emit_namestring(acpi_device_path_join(dev, "PTS"));
	acpigen_write_method_end();
	acpigen_write_scope_end();

	printk(BIOS_INFO, "%s.%s %s\n", parent_scope, dev_name, dev_path(dev));
	ssdt_done = true;
}

static const char *dtbt_acpi_name(const struct device *dev)
{
	return "DTBT";
}

static void dtbt_enable(struct device *dev)
{
	if (!is_dev_enabled(dev) || enable_done)
		return;

	printk(BIOS_INFO, "dTBT controller found at %s\n", dev_path(dev));

	// XXX: Recommendation is to set SL1 ("User Authorization")
	printk(BIOS_DEBUG, "dTBT set security level SL0\n");
	/* Set security level */
	dtbt_cmd(dev, PCIE2TBT_SET_SECURITY_LEVEL, SEC_LEVEL_NONE, MBOX_TIMEOUT_MS);

	if (acpi_is_wakeup_s3()) {
		printk(BIOS_DEBUG, "dTBT SX exit\n");
		dtbt_cmd(dev, PCIE2TBT_SX_EXIT_TBT_CONNECTED, 0, MBOX_TIMEOUT_MS);
		/* Read TBT2PCIE register, verify not invalid */
		if (pci_read_config32(dev, TBT2PCIE) == 0xffffffff)
			printk(BIOS_ERR, "dTBT S3 resume failure.\n");
	} else {
		printk(BIOS_DEBUG, "dTBT set boot on\n");
		dtbt_cmd(dev, PCIE2TBT_BOOT_ON, 0, MBOX_TIMEOUT_MS);
		printk(BIOS_DEBUG, "dTBT set USB on\n");
		dtbt_cmd(dev, PCIE2TBT_USB_ON, 0, MBOX_TIMEOUT_MS);
	}
	enable_done = true;
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
	.enable           = dtbt_enable
};

/* We only want to match the (first) bridge device */
static const unsigned short pci_device_ids[] = {
	AR_2C_BRG,
	AR_4C_BRG,
	AR_LP_BRG,
	AR_4C_C0_BRG,
	AR_2C_C0_BRG,
	TR_2C_BRG,
	TR_4C_BRG,
	TR_DD_BRG,
	MR_2C_BRG,
	MR_4C_BRG,
	BR_4_BRG,
	BR_5_BRG,
	0
};

static const struct pci_driver intel_dtbt_driver __pci_driver = {
	.ops		= &dtbt_device_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};

struct chip_operations drivers_intel_dtbt_ops = {
	.name = "Intel Discrete Thunderbolt",
};
