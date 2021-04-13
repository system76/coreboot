/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <option.h>
#include <pc80/keyboard.h>
#include <soc/ramstage.h>
#include "gpio.h"

void mainboard_silicon_init_params(FSP_SIL_UPD *params) {
	/* Configure pads prior to SiliconInit() in case there's any
	 * dependencies during hardware initialization. */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

static int ec_cmd(u8 data) {
	int i = 1000000;
	while ((inb(0x66) & 2) == 2 && i > 0) {
		i -= 1;
	}
	if (i == 0) {
		return 1;
	} else {
		outb(data, 0x66);
		return 0;
	}
}

static void mainboard_init(struct device *dev) {
	printk(BIOS_INFO, "system76: keyboard init\n");
	pc_keyboard_init(NO_AUX_DEVICE);

	// Rescan for EC devices - fixes camera toggle
	printk(BIOS_INFO, "system76: EC init\n");
	if (ec_cmd(0xA8)) {
		printk(BIOS_ERR, "system76: failed to send EC command 0xA8\n");
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
