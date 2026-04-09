/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <option.h>
#include <soc/platform_descriptors.h>
#include <variants.h>

enum cmos_power_profile get_power_profile(enum cmos_power_profile fallback)
{
	const unsigned int power_profile = get_uint_option("power_profile", fallback);
	return power_profile < NUM_POWER_PROFILES ? power_profile : fallback;
}

void mb_pre_fspm(FSP_M_CONFIG *mcfg)
{
	const struct soc_amd_gpio *pads;
	size_t num;

	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);

	/* Update Power Profile based on CFR/CMOS settings */
	switch (get_power_profile(PP_POWER_SAVER)) {
	case PP_POWER_SAVER:
		mcfg->system_configuration = 1;		// 10W TDP
		break;
	case PP_BALANCED:
		mcfg->system_configuration = 2;		// 15W TDP
		break;
	case PP_PERFORMANCE:
		mcfg->system_configuration = 3;		// 25W TDP
		break;
	}
}
