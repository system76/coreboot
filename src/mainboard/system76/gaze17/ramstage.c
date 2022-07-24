/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <variant/gpio.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	params->CnviRfResetPinMux = 0x194CE404; // GPP_F4
	params->CnviClkreqPinMux = 0x394CE605; // GPP_F5

	params->PchSerialIoI2cSdaPinMux[0] = 0x1947c404; // GPP_H4
	params->PchSerialIoI2cSclPinMux[0] = 0x1947a405; // GPP_H5
	params->PchSerialIoI2cSdaPinMux[1] = 0x1947c606; // GPP_H6
	params->PchSerialIoI2cSclPinMux[1] = 0x1947a607; // GPP_H7

	params->SataPortDevSlpPinMux[0] = 0x59673e0c; // GPP_H12
	params->SataPortDevSlpPinMux[1] = 0x5967400d; // GPP_H13

	variant_configure_gpios();
}
