/* SPDX-License-Identifier: GPL-2.0-only */

//TODO: do not require this to be included in mainboard bootblock.c

#include <console/console.h>
#include <delay.h>
#include <gpio.h>

static void dgpu_power_enable(int onoff)
{
	printk(BIOS_DEBUG, "system76: DGPU power %d\n", onoff);
	if (onoff) {
		gpio_set(DGPU_RST_N, 0);
		mdelay(4);
		gpio_set(DGPU_PWR_EN, 1);
		mdelay(4);
		gpio_set(DGPU_RST_N, 1);
	} else {
		gpio_set(DGPU_RST_N, 0);
		mdelay(4);
		gpio_set(DGPU_PWR_EN, 0);
	}
	mdelay(50);
}
