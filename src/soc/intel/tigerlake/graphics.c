/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/graphics.h>
#include <soc/ramstage.h>

const struct i915_gpu_controller_info *
intel_igd_get_controller_info(const struct device *device)
{
	struct soc_intel_tigerlake_config *chip = device->chip_info;
	return &chip->gfx;
}
