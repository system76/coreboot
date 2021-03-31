/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <option.h>
#include <pc80/keyboard.h>
#include <soc/ramstage.h>
#include "gpio.h"

void mainboard_silicon_init_params(FSPS_UPD *supd) {
	/* Configure pads prior to SiliconInit() in case there's any
	 * dependencies during hardware initialization. */
	cnl_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

static u8 superio_read(u8 reg) {
	outb(reg, 0x2E);
	return inb(0x2F);
}

static void superio_write(u8 reg, u8 value) {
	outb(reg, 0x2E);
	outb(value, 0x2F);
}

static u8 d2_read(u8 reg) {
	superio_write(0x2E, reg);
	return superio_read(0x2F);
}

static void d2_write(u8 reg, u8 value) {
	superio_write(0x2E, reg);
	superio_write(0x2F, value);
}

static u8 i2ec_read(u16 addr) {
	d2_write(0x11, (u8)(addr >> 8));
	d2_write(0x10, (u8)addr);
	return d2_read(0x12);
}

static void i2ec_write(u16 addr, u8 value) {
	d2_write(0x11, (u8)(addr >> 8));
	d2_write(0x10, (u8)addr);
	d2_write(0x12, value);
}

static void mainboard_init(struct device *dev) {
	printk(BIOS_INFO, "system76: keyboard init\n");
	pc_keyboard_init(NO_AUX_DEVICE);

	printk(BIOS_INFO, "system76: EC init\n");

	// Black magic - force enable camera toggle
	u16 addr = 0x01CA;
	u8 value = i2ec_read(addr);
	if ((value & (1 << 2)) == 0) {
		printk(BIOS_INFO, "system76: enabling camera toggle\n");
		i2ec_write(addr, value | (1 << 2));
	} else {
		printk(BIOS_INFO, "system76: camera toggle already enabled\n");
	}
}

static void mainboard_enable(struct device *dev) {
	dev->ops->init = mainboard_init;

	// Configure pad for DisplayPort
	uint32_t config = 0x44000200;

	uint8_t nvram = 0;
	if (get_option(&nvram, "DisplayPort_Output") == CB_SUCCESS) {
		if (nvram) {
			config |= 1;
		}
	}

	if (nvram) {
		printk(BIOS_INFO, "system76: DisplayPort_Output set to USB-C: 0x%x\n", config);
	} else {
		printk(BIOS_INFO, "system76: DisplayPort_Output set to Mini_DisplayPort: 0x%x\n", config);
	}

	struct pad_config displayport_gpio_table[] = {
		/* PS8338B_SW */
		_PAD_CFG_STRUCT(GPP_A22, config, 0x0),
	};
	gpio_configure_pads(displayport_gpio_table, ARRAY_SIZE(displayport_gpio_table));
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
