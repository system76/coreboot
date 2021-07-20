/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <variant/gpio.h>
#include "variant.h"

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	variant_silicon_init_params(params);

	// Low latency legacy I/O
	params->PchLegacyIoLowLatency = 1;

	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
