/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <gpio.h>
#include "chip.h"
#include "gpu.h"

void nvidia_set_power(const struct nvidia_gpu_config *config)
{
	if (!config->power_gpio || !config->reset_gpio) {
		printk(BIOS_ERR, "%s: GPU_PWR_EN and GPU_RST# must be set\n", __func__);
		return;
	}

	printk(BIOS_DEBUG, "%s: GPU_PWR_EN = %d\n", __func__, config->power_gpio);
	printk(BIOS_DEBUG, "%s: GPU_RST# = %d\n", __func__, config->reset_gpio);

	gpio_set(config->reset_gpio, 0);
	mdelay(10);

	if (config->enable) {
		gpio_set(config->power_gpio, 1);
		mdelay(25);
		gpio_set(config->reset_gpio, 1);
	} else {
		gpio_set(config->power_gpio, 0);
	}

	mdelay(10);
}
