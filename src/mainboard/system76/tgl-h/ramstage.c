/* SPDX-License-Identifier: GPL-2.0-only */

#include <smbios.h>
#include <soc/ramstage.h>
#include <variant/gpio.h>
#include "variant.h"

smbios_wakeup_type smbios_system_wakeup_type(void)
{
	return SMBIOS_WAKEUP_TYPE_POWER_SWITCH;
}

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	variant_silicon_init_params(params);

	params->PchLegacyIoLowLatency = 1;

	variant_configure_gpios();
}
