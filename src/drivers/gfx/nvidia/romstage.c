/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <gpio.h>
#include <timer.h>
#include "chip.h"
#include "gpu.h"

void nvidia_set_power(const struct nvidia_gpu_config *config)
{
	if (!config->power_gpio || !config->reset_gpio) {
		printk(BIOS_ERR, "%s: GPU_PWR_EN, GPU_RST# must be set\n", __func__);
		return;
	}

	printk(BIOS_DEBUG, "%s: GPU_PWR_EN = %d\n", __func__, config->power_gpio);
	printk(BIOS_DEBUG, "%s: GPU_PWRGD = %d\n", __func__, config->power_good_gpio);
	printk(BIOS_DEBUG, "%s: GPU_RST# = %d\n", __func__, config->reset_gpio);

	gpio_set(config->reset_gpio, 0);
	mdelay(10);

	if (config->enable) {
		gpio_set(config->power_gpio, 1);
		if (config->power_good_gpio) {
			if (wait_ms(50, gpio_get(config->power_good_gpio) == 1)) {
				printk(BIOS_INFO, "dGPU powered on\n");
				gpio_set(config->reset_gpio, 1);
			} else {
				printk(BIOS_ERR, "dGPU failed to power on, turning off\n");
				gpio_set(config->power_gpio, 0);
			}
		} else {
			mdelay(50);
			gpio_set(config->reset_gpio, 1);
		}
	} else {
		gpio_set(config->power_gpio, 0);
	}

	mdelay(10);
}
