/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <baseboard/panel.h>
#include <console/console.h>
#include <fw_config.h>
#include <variants.h>

enum audio_amplifier_id get_audio_amp_id(void)
{
	if (fw_config_probe(FW_CONFIG(AUDIO_AMPLIFIER, AUDIO_AMPLIFIER_TAS2563)))
		return AUD_AMP_ID_TAS2563;

	return AUD_AMP_ID_UNKNOWN;
}

void fw_config_get_mainboard_override(uint64_t *fw_config)
{
	/* Handle unprovisioned fw_config. */
	if (*fw_config == UNDEFINED_FW_CONFIG) {
		/* Reset fw_config to a default value with boot-necessary fields.
		   For now, setting to 0 is sufficient. */
		*fw_config = 0;
	}

	uint32_t id = panel_id();
	printk(BIOS_INFO, "Overriding fw_config PANEL_ID with %u\n", id);
	fw_config_value_set_field(fw_config, FW_CONFIG_FIELD(PANEL_ID), id);
}
