/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <device/pci.h>
#include <soc/ramstage.h>
#include "gpio.h"

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/* Configure pads prior to SiliconInit() in case there's any
	 * dependencies during hardware initialization. */
	cnl_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

static void dgpu_read_resources(struct device *dev) {
	printk(BIOS_INFO, "system76: dgpu_read_resources %s\n", dev_path(dev));

    pci_dev_read_resources(dev);

	int bar;
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

static struct device_operations dgpu_pci_ops_dev = {
	.read_resources   = dgpu_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = pci_rom_write_acpi_tables,
	.acpi_fill_ssdt    = pci_rom_ssdt,
#endif
	.init             = pci_dev_init,
	.ops_pci          = &pci_dev_ops_pci,
};

static void dgpu_above_4g(void *unused) {
	struct device *pdev;

	// Find PEG0
	pdev = pcidev_on_root(1, 0);
	if (!pdev) {
		printk(BIOS_ERR, "system76: failed to find PEG0\n");
		return;
	}
	printk(BIOS_INFO, "system76: PEG0 at %p, %04x:%04x\n", pdev, pdev->vendor, pdev->device);

    int fn;
    for (fn = 0; fn < 8; fn++) {
	    struct device *dev;

        // Find DGPU functions
        dev = pcidev_path_behind(pdev->link_list, PCI_DEVFN(0, fn));
        if (dev) {
            printk(BIOS_INFO, "system76: DGPU fn %d at %p, %04x:%04x\n", fn, dev, dev->vendor, dev->device);
            dev->ops = &dgpu_pci_ops_dev;
        } else {
            printk(BIOS_ERR, "system76: failed to find DGPU fn %d\n", fn);
        }
    }
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_ENTRY, dgpu_above_4g, NULL);
