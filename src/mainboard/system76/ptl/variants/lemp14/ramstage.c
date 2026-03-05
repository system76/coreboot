/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	// TODO: Pin Mux settings

	// Enable TCP2 USB-A conversion
	// BIT 0:3 is mapping to PCH XHCI USB2 port
	// BIT 4:5 is reserved
	// BIT 6 is orientational
	// BIT 7 is enable
	params->EnableTcssCovTypeA[2] = 0x83;

	// Disable reporting CPU C10 state over eSPI (causes LED flicker).
	params->PchEspiHostC10ReportEnable = 0;
}
