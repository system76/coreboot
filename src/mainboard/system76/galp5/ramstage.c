/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include "gpio.h"

void mainboard_silicon_init_params(FSP_S_CONFIG *params) {
	// CPU RP Config
	params->CpuPcieRpAdvancedErrorReporting[0] = 0;
	params->CpuPcieRpLtrEnable[0] = 1;
	params->CpuPcieRpPtmEnabled[0] = 0;

	// IOM config
	params->PchUsbOverCurrentEnable = 0;
	params->PortResetMessageEnable[5] = 1; // J_TYPEC2
	params->UsbTcPortEn = 1;

	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
