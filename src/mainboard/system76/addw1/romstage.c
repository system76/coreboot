/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/gfx/nvidia/gpu.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <variant/gpio.h>

static const struct cnl_mb_cfg memcfg = {
	.spd[0] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xa0},
	},
	.spd[2] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xa4},
	},
	.rcomp_resistor = { 121, 75, 100 },
	.rcomp_targets = { 50, 25, 20, 20, 26 },
	.dq_pins_interleaved = 1,
	.vref_ca_config = 2,
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct nvidia_gpu_config config = {
		.power_gpio = DGPU_PWR_EN,
		.reset_gpio = DGPU_RST_N,
		.enable = true,
	};

	// Enable dGPU power
	nvidia_set_power(&config);

	// Set primary display to internal graphics
	memupd->FspmConfig.PrimaryDisplay = 0;

	cannonlake_memcfg_init(&memupd->FspmConfig, &memcfg);
}
