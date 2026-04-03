/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <device/device.h>

static void mainboard_init(void *chip_info)
{
	/* TODO: Perform mainboard initialization */
}

static void mainboard_fill_ssdt(const struct device *dev)
{
	/* TODO: Add mainboard specific SSDT */
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
