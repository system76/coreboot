/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <boardid.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	const uint32_t id = board_id();
	if (id != BOARD_ID_UNKNOWN && id >= 2) /* proto1 = 0, proto2 = 1 */
		return;

	config->ext_fivr_settings.configure_ext_fivr = 1;
	config->ext_fivr_settings.v1p05_enable_bitmap =
			FIVR_ENABLE_ALL_SX & ~FIVR_ENABLE_S0;
	config->ext_fivr_settings.vnn_enable_bitmap =
			FIVR_ENABLE_ALL_SX;
	config->ext_fivr_settings.vnn_sx_enable_bitmap =
			FIVR_ENABLE_ALL_SX;
	config->ext_fivr_settings.v1p05_supported_voltage_bitmap =
			FIVR_VOLTAGE_NORMAL;
	config->ext_fivr_settings.vnn_supported_voltage_bitmap =
			FIVR_VOLTAGE_MIN_ACTIVE;
	config->ext_fivr_settings.v1p05_voltage_mv = 1050;
	config->ext_fivr_settings.vnn_voltage_mv = 780;
	config->ext_fivr_settings.vnn_sx_voltage_mv = 1050;
	config->ext_fivr_settings.v1p05_icc_max_ma = 500;
	config->ext_fivr_settings.vnn_icc_max_ma = 500;
	printk(BIOS_INFO, "Configured External FIVR\n");
}
