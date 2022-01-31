/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>

#include "gpio.h"

void bootblock_mainboard_early_init(void)
{
	configure_early_gpio_pads();
}
