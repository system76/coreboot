/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <gpio.h>
#include <mainboard/gpio.h>

#include <drivers/system76/dgpu/bootblock.c>

void bootblock_mainboard_early_init(void)
{
	mainboard_configure_early_gpios();
	dgpu_power_enable(1);
}
