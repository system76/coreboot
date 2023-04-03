/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	// Disable AER to fix suspend failing with some SSDs.
	params->CpuPcieRpAdvancedErrorReporting[0] = 0;
	params->CpuPcieRpLtrEnable[0] = 1;
	params->CpuPcieRpPtmEnabled[0] = 0;

	// IOM config
	params->PchUsbOverCurrentEnable = 0;
	params->PortResetMessageEnable[5] = 1; // J_TYPEC2

	// Enable reporting CPU C10 state over ESPI
	params->PchEspiHostC10ReportEnable = 1;
}
