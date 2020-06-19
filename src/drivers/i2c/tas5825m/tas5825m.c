/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include "chip.h"

static int tas5825m_setup(struct device *dev) {
	struct drivers_i2c_tas5825m_config *config = dev->chip_info;

	if (!config) {
		printk(BIOS_ERR, "tas5825m: failed to find config\n");
		return -1;
	}

	//TODO: handle errors
	int res;
	#define R(F, E) { \
		res = F; \
		if (res < 0) { \
			printk(BIOS_ERR, "tas5825m: %s\n", E); \
			return res; \
		} \
	}

	// Set page to 0
	R(smbus_write_byte(dev, 0x00, 0x00), "failed to set page");

	// Set book to 0
	R(smbus_write_byte(dev, 0x7F, 0x00), "failed to set book");

	// Get DIE_ID
	R(smbus_read_byte(dev, 0x67), "failed to read die id");
	printk(BIOS_DEBUG, "tas5825m: DIE_ID=%02X\n", res);

	#undef R
	return 0;
}

static void tas5825m_init(struct device *dev) {
	if (dev->enabled && dev->path.type == DEVICE_PATH_I2C &&
		ops_smbus_bus(get_pbus_smbus(dev))) {
		printk(BIOS_DEBUG, "tas5825m at %s\n", dev_path(dev));
		int res = tas5825m_setup(dev);
		if (res) {
			printk(BIOS_ERR, "tas5825m init failed: %d\n", res);
		} else {
			printk(BIOS_DEBUG, "tas5825m init successful\n");
		}
	}
}

static struct device_operations tas5825m_operations = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.init			= tas5825m_init,
};

static void tas5825m_enable_dev(struct device *dev) {
	dev->ops = &tas5825m_operations;
}

struct chip_operations drivers_i2c_tas5825m_ops = {
	CHIP_NAME("TI TAS5825M Amplifier")
	.enable_dev = tas5825m_enable_dev,
};
