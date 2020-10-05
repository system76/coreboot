/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/path.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/smbus.h>
#include <device/smbus_host.h>
#include "smbuslib.h"

static int lsmbus_read_byte(struct device *dev, u8 address)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;
	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);
	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(struct device *dev, u8 address, u8 data)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);
	return do_smbus_write_byte(res->base, device, address, data);
}

static struct smbus_bus_operations lops_smbus_bus = {
	.read_byte	= lsmbus_read_byte,
	.write_byte	= lsmbus_write_byte,
};

static void pch_smbus_init(struct device *dev)
{
	struct resource *res;

	/* Enable clock gating */
	pci_update_config32(dev, 0x80,
		~((1 << 8) | (1 << 10) | (1 << 12) | (1 << 14)), 0);

	/* Set Receive Slave Address */
	res = find_resource(dev, PCI_BASE_ADDRESS_4);
	if (res)
		smbus_set_slave_addr(res->base, SMBUS_SLAVE_ADDR);
}

static void smbus_read_resources(struct device *dev)
{
	struct resource *res = new_resource(dev, PCI_BASE_ADDRESS_4);
	res->base = SMBUS_IO_BASE;
	res->size = 32;
	res->limit = res->base + res->size - 1;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_RESERVE |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	/* Also add MMIO resource */
	res = pci_get_resource(dev, PCI_BASE_ADDRESS_0);
}

static struct device_operations smbus_ops = {
	.read_resources		= smbus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_smbus,
	.init			= pch_smbus_init,
	.ops_pci		= &pci_dev_ops_pci,
	.ops_smbus_bus		= &lops_smbus_bus,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_CNL_SMBUS,
	PCI_DEVICE_ID_INTEL_CNP_H_SMBUS,
	PCI_DEVICE_ID_INTEL_SPT_LP_SMBUS,
	PCI_DEVICE_ID_INTEL_SPT_H_SMBUS,
	PCI_DEVICE_ID_INTEL_LWB_SMBUS_SUPER,
	PCI_DEVICE_ID_INTEL_LWB_SMBUS,
	PCI_DEVICE_ID_INTEL_ICP_LP_SMBUS,
	PCI_DEVICE_ID_INTEL_CMP_SMBUS,
	PCI_DEVICE_ID_INTEL_CMP_H_SMBUS,
	PCI_DEVICE_ID_INTEL_TGP_LP_SMBUS,
	PCI_DEVICE_ID_INTEL_MCC_SMBUS,
	PCI_DEVICE_ID_INTEL_JSP_SMBUS,
	0
};

static const struct pci_driver pch_smbus __pci_driver = {
	.ops	 = &smbus_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices	 = pci_device_ids,
};
