/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	// CPU RP Config
	params->CpuPcieRpAdvancedErrorReporting[0] = 0;
	params->CpuPcieRpLtrEnable[0] = 1;
	params->CpuPcieRpPtmEnabled[0] = 0;

	// IOM config
	params->PchUsbOverCurrentEnable = 0;
	params->PortResetMessageEnable[2] = 1; // J_TYPEC1
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
