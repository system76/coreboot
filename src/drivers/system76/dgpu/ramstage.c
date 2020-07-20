/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <device/pci.h>

static void dgpu_read_resources(struct device *dev)
{
	int bar;

	printk(BIOS_INFO, "system76: %s %s\n", __func__, dev_path(dev));

	pci_dev_read_resources(dev);

	// Find all BARs on DGPU, mark them above 4g if prefetchable
	for (bar = PCI_BASE_ADDRESS_0; bar <= PCI_BASE_ADDRESS_5; bar += 4) {
		printk(BIOS_INFO, "  BAR at 0x%02x\n", bar);

		struct resource *res;
		res = probe_resource(dev, bar);
		if (res) {
			if (res->flags & IORESOURCE_PREFETCH) {
				printk(BIOS_INFO, "    marked above 4g\n");
				res->flags |= IORESOURCE_ABOVE_4G;
			} else {
				printk(BIOS_INFO, "    not prefetch\n");
			}
		} else {
			printk(BIOS_INFO, "    not found\n");
		}
	}
}

static void dgpu_enable_resources(struct device *dev)
{
	printk(BIOS_INFO, "system76: %s %s\n", __func__, dev_path(dev));

	dev->subsystem_vendor = CONFIG_SUBSYSTEM_VENDOR_ID;
	dev->subsystem_device = CONFIG_SUBSYSTEM_DEVICE_ID;
	printk(BIOS_INFO, "  subsystem <- %04x/%04x\n",
		dev->subsystem_vendor, dev->subsystem_device);
	pci_write_config32(dev, 0x40,
		((dev->subsystem_device & 0xffff) << 16) |
		(dev->subsystem_vendor & 0xffff));

	pci_dev_enable_resources(dev);
}

static struct device_operations dgpu_pci_ops_dev = {
	.read_resources		= dgpu_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= dgpu_enable_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables	= pci_rom_write_acpi_tables,
	.acpi_fill_ssdt		= pci_rom_ssdt,
#endif
	.init			= pci_dev_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static void dgpu_above_4g(void *unused)
{
	struct device *pdev;
	int fn;

	// Find PEGP
	pdev = pcidev_on_root(CONFIG_DRIVERS_SYSTEM76_DGPU_DEVICE, 0);
	if (!pdev) {
		printk(BIOS_ERR, "system76: failed to find PEGP\n");
		return;
	}
	printk(BIOS_INFO, "system76: PEGP at %p, %04x:%04x\n",
		pdev, pdev->vendor, pdev->device);

	for (fn = 0; fn < 8; fn++) {
		struct device *dev;

		// Find DGPU functions
		dev = pcidev_path_behind(pdev->link_list, PCI_DEVFN(0, fn));
		if (dev) {
			printk(BIOS_INFO, "system76: DGPU fn %d at %p, %04x:%04x\n",
				fn, dev, dev->vendor, dev->device);
			dev->ops = &dgpu_pci_ops_dev;
		} else {
			printk(BIOS_ERR, "system76: failed to find DGPU fn %d\n",
				fn);
		}
	}
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_ENTRY, dgpu_above_4g, NULL);
