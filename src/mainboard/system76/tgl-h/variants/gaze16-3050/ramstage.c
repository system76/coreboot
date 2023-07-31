/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	params->PchLegacyIoLowLatency = 1;

	// PEG0 Config
	params->CpuPcieRpAdvancedErrorReporting[0] = 0;
	params->CpuPcieRpLtrEnable[0] = 1;
	params->CpuPcieRpPtmEnabled[0] = 0;

	// PEG2 Config
	params->CpuPcieRpAdvancedErrorReporting[2] = 0;
	params->CpuPcieRpLtrEnable[2] = 1;
	params->CpuPcieRpPtmEnabled[2] = 0;

	// Remap PEG2 as PEG1
	params->CpuPcieRpFunctionSwap = 1;

	// Enable reporting CPU C10 state over ESPI
	params->PchEspiHostC10ReportEnable = 1;
}
