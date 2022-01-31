/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include "ec.h"

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

/*
 * mainboard_enable is executed as first thing after
 * enumerate_buses().
 */
static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
