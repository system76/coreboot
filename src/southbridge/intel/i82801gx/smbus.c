/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/path.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/smbus_host.h>
#include <southbridge/intel/common/smbus_ops.h>
#include "i82801gx.h"

static struct device_operations smbus_ops = {
	.read_resources		= smbus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_smbus,
	.enable			= i82801gx_enable,
	.ops_smbus_bus		= &lops_smbus_bus,
	.ops_pci		= &pci_dev_ops_pci,
};

/* 82801GB/GR/GDH/GBM/GHM/GU (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH/ICH7-U) */
static const struct pci_driver i82801gx_smbus __pci_driver = {
	.ops	= &smbus_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= 0x27da,
};
