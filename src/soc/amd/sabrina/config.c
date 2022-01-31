/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <amdblocks/chip.h>
#include <device/device.h>
#include "chip.h"

const struct soc_amd_common_config *soc_get_common_config(void)
{
	/* config_of_soc calls die() internally if cfg was NULL, so no need to re-check */
	const struct soc_amd_sabrina_config *cfg = config_of_soc();
	return &cfg->common_config;
}
