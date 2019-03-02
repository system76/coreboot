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

static void mainboard_enable(struct device *dev) {
	pc_keyboard_init(NO_AUX_DEVICE);

	uint32_t config = 0x44000200;

	uint8_t nvram = 0;
	if (get_option(&nvram, "DisplayPort_Output") == CB_SUCCESS) {
		if (nvram) {
			config |= 1;
		}
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
