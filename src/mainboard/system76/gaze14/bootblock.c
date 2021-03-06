/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <gpio.h>
#include "gpio.h"

#include <drivers/system76/dgpu/bootblock.c>

void bootblock_mainboard_init(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
	dgpu_power_enable(1);
}
