/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <gpio.h>
#include "gpio.h"
#if CONFIG(DRIVERS_SYSTEM76_DGPU)
	#include <drivers/system76/dgpu/bootblock.c>
#endif

void bootblock_mainboard_init(void) {
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
#if CONFIG(DRIVERS_SYSTEM76_DGPU)
	dgpu_power_enable(1);
#endif
}
