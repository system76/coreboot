/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <symbols.h>
#include <cbmem.h>
#include <mainboard/addressmap.h>

static void qemu_riscv_domain_read_resources(struct device *dev)
{
	struct resource *res;
	int index = 0;

	/* PCI I/O port window */
	res = new_resource(dev, index++);
	res->limit = 0xffff;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED;

	/* 32-bit PCI MMIO window */
	res = new_resource(dev, index++);
	res->base = QEMU_VIRT_PCIE_MMIO_BASE;
	res->limit = QEMU_VIRT_PCIE_MMIO_LIMIT;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;

	/*
	 * NOTE: 64-bit MMIO window (0x300000000-0x3ffffffff) is omitted
	 * because OpenSBI 1.1 (bundled with coreboot) does not add PMP
	 * entries for it, causing S-mode load access faults.  All BARs
	 * will be assigned in the 32-bit window which is plenty for the
	 * QEMU virt machine's typical device set.
	 */

	/* ECAM config space (fixed MMIO) */
	mmio_range(dev, index++, QEMU_VIRT_PCIE_ECAM, QEMU_VIRT_PCIE_ECAM_SIZE);

	/* DRAM */
	ram_from_to(dev, index++, (uintptr_t)_dram, cbmem_top());
}

struct device_operations qemu_riscv_pci_domain_ops = {
	.read_resources    = qemu_riscv_domain_read_resources,
	.set_resources     = pci_domain_set_resources,
	.scan_bus          = pci_host_bridge_scan_bus,
};

static void mainboard_enable(struct device *dev)
{
	if (!dev)
		die("No dev0; die\n");
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
