/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	// IOM config
	params->PchUsbOverCurrentEnable = 0;
	params->PortResetMessageEnable[2] = 1; // J_TYPEC1
	params->UsbTcPortEn = 1;

	mainboard_configure_gpios();
}
