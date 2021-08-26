/* SPDX-License-Identifier: GPL-2.0-only */

#include "dgpu.h"
#include <console/console.h>
#include <delay.h>
#include <gpio.h>

void dgpu_power_enable(const struct system76_dgpu_config *config)
{
	if (!config->enable_gpio || !config->reset_gpio) {
		printk(BIOS_ERR, "%s: GPU_PWR_EN and GPU_RST# must be set\n", __func__);
		return;
	}

	printk(BIOS_DEBUG, "%s: DGPU power %s\n", __func__, config->enable ? "on" : "off");

	// Put GPU into reset
	gpio_set(config->reset_gpio, 0);
	mdelay(4);

	if (config->enable) {
		// Enable GPU power
		gpio_set(config->enable_gpio, 1);
		mdelay(4);
		// Take GPU out of reset
		gpio_set(config->reset_gpio, 1);
	} else {
		// Disable GPU power
		gpio_set(config->enable_gpio, 0);
		// Leave GPU in reset
	}

	mdelay(50);
}
