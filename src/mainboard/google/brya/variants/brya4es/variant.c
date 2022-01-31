/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <fw_config.h>
#include <baseboard/variants.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	config->CnviBtAudioOffload = fw_config_probe(FW_CONFIG(AUDIO, MAX98373_ALC5682_SNDW));
}
