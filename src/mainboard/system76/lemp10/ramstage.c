/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	params->CpuPcieRpAdvancedErrorReporting[0] = 0;

	mainboard_configure_gpios();
}
