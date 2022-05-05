/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include "gpio.h"

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	params->CpuPcieRpAdvancedErrorReporting[0] = 0;

	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
