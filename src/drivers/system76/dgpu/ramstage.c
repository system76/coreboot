/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#define NVIDIA_SUBSYSTEM_ID_OFFSET 0x40

static void dgpu_read_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "system76: %s: %s\n", __func__, dev_path(dev));

	pci_dev_read_resources(dev);

	// Find all BARs on DGPU, mark them above 4g if prefetchable
	for (unsigned int bar = PCI_BASE_ADDRESS_0; bar <= PCI_BASE_ADDRESS_5; bar += 4) {
		struct resource *res = probe_resource(dev, bar);

		if (res) {
			if (res->flags & IORESOURCE_PREFETCH) {
				printk(BIOS_DEBUG, "  BAR at 0x%02x marked above 4g\n", bar);
				res->flags |= IORESOURCE_ABOVE_4G;
			} else {
				printk(BIOS_DEBUG, "  BAR at 0x%02x not prefetch\n", bar);
			}
		} else {
			printk(BIOS_DEBUG, "  BAR at 0x%02x not found\n", bar);
		}
	}
}

static void dgpu_set_subsystem(struct device *dev, unsigned int vendor, unsigned int device)
{
	pci_write_config32(dev, NVIDIA_SUBSYSTEM_ID_OFFSET,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations dgpu_device_ops_pci = {
	.set_subsystem = dgpu_set_subsystem,
};

static struct device_operations dgpu_device_ops = {
	.read_resources		= dgpu_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= pci_dev_init,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables	= pci_rom_write_acpi_tables,
	.acpi_fill_ssdt		= pci_rom_ssdt,
#endif
	.ops_pci		= &dgpu_device_ops_pci,
};

static void dgpu_enable(struct device *dev)
{
	u16 vendor = pci_read_config16(dev, PCI_VENDOR_ID);

	if (dev->path.type != DEVICE_PATH_PCI || vendor != PCI_VENDOR_ID_NVIDIA) {
		printk(BIOS_ERR, "%s: Not an NVIDIA device (%04x)\n", dev_path(dev), vendor);
		return;
	}

	dev->ops = &dgpu_device_ops;
}

struct chip_operations drivers_system76_dgpu_ops = {
	CHIP_NAME("System76 hybrid graphics")
	.enable_dev = dgpu_enable
};
