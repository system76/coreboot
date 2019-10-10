/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 System76.
 * Copyright (C) 2017-2018 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static void slot_dev_read_resources(struct device *dev)
{
	struct resource *resource;

	resource = new_resource(dev, 0x10);
	resource->size = 1 << 28;
	resource->align = 22;
	resource->gran = 22;
	resource->limit = 0xffffffff;
	resource->flags |= IORESOURCE_MEM;

	resource = new_resource(dev, 0x14);
	resource->size = 1 << 28;
	resource->align = 22;
	resource->gran = 22;
	resource->limit = 0xffffffff;
	resource->flags |= IORESOURCE_MEM | IORESOURCE_PREFETCH;

	resource = new_resource(dev, 0x18);
	resource->size = 1 << 13;
	resource->align = 12;
	resource->gran = 12;
	resource->limit = 0xffff;
	resource->flags |= IORESOURCE_IO;
}

static struct device_operations slot_dev_ops = {
	.read_resources   = slot_dev_read_resources,
};

static bool tbt_is_hotplug_bridge(struct device *dev) {
	return PCI_SLOT(dev->path.pci.devfn) == 1;
}

static void tbt_pciexp_scan_bridge(struct device *dev) {
	printk(BIOS_DEBUG, "%s: %s: scan bridge\n", __func__, dev_path(dev));

	bool is_hotplug = tbt_is_hotplug_bridge(dev);
	if (is_hotplug) {
		/* Add hotplug buses, must happen before bus scan */
		printk(BIOS_DEBUG, "%s: %s: add hotplug buses\n", __func__, dev_path(dev));
		dev->hotplug_buses = 32;
	}

	/* Normal PCIe Scan */
	pciexp_scan_bridge(dev);

	if (is_hotplug) {
		/* Add dummy slot to preserve resources, must happen after bus scan */
		printk(BIOS_DEBUG, "%s: %s: add dummy device\n", __func__, dev_path(dev));
		struct device *slot;
		struct device_path slot_path = { .type = DEVICE_PATH_NONE };
		slot = alloc_dev(dev->link_list, &slot_path);
		slot->ops = &slot_dev_ops;
	}
}

static struct pci_operations pcie_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = 0,
	.scan_bus         = tbt_pciexp_scan_bridge,
	.enable           = 0,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &pcie_ops,
};

static const unsigned short pcie_device_ids[] = {
	// JHL7540 Thunderbolt 3 Bridge
	0x15e7,
	0
};

static const struct pci_driver tbt_pcie __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.devices	= pcie_device_ids,
};
