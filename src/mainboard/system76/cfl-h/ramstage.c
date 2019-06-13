/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 System76
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <gpio.h>
#include <pc80/keyboard.h>
#include <soc/ramstage.h>
#include <variant/gpio.h>

#ifdef GPU_RESET

#define DGPU_RSTN GPP_F22
#define DGPU_PWR_EN GPP_F23
#define DGPU_PWRGD GPP_K22

static void gpu_reset(void) {
	// Set DGPU_PWR_EN and Wait for DGPU_PWRGD
	printk(BIOS_INFO, "system76: DGPU reset start\n");
	gpio_set(DGPU_RSTN, 0);

	printk(BIOS_INFO, "system76: DGPU disable power\n");
	gpio_set(DGPU_PWR_EN, 0);
	while (gpio_get(DGPU_PWRGD)) {
		printk(BIOS_INFO, "system76: DGPU wait for disabled power\n");
		mdelay(4);
	}

	printk(BIOS_INFO, "system76: DGPU enable power\n");
	gpio_set(DGPU_PWR_EN, 1);
	while (! gpio_get(DGPU_PWRGD)) {
		printk(BIOS_INFO, "system76: DGPU wait for enabled power\n");
		mdelay(4);
	}

	printk(BIOS_INFO, "system76: DGPU reset finished\n");
	gpio_set(DGPU_RSTN, 1);
}

#endif

void mainboard_silicon_init_params(FSP_S_CONFIG *params) {
	/* Configure pads prior to SiliconInit() in case there's any
	 * dependencies during hardware initialization. */
	cnl_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

static void mainboard_init(struct device *dev) {
	printk(BIOS_INFO, "system76: keyboard init\n");
	pc_keyboard_init(NO_AUX_DEVICE);
}

static void mainboard_enable(struct device *dev) {
#ifdef GPU_RESET
	gpu_reset();
#endif

	dev->ops->init = mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
